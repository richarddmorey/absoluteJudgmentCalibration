/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2002 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU Lesser General Public License version 2.1 as published by the Free
*   Software Foundation and appearing in the file LICENSE.LGPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any 32-bit protected mode environment
*
* Description:  Module to implement the code to test the SciTech SNAP
*               Graphics Architecture.
*
****************************************************************************/

#include "snap/gasdk.h"

/*---------------------------- Global Variables ---------------------------*/

static ibool        forceSoftwareCount = 0;
ibool               softwareOnly = false;
N_uint16            cntMode;
N_int32             x,y,maxX,maxY,maxPage,activePage,visualPage,maxMem;
N_int32             virtualX,virtualY,bytesPerLine,bytesPerPixel;
N_int32             isStereo,useBlueCode,blueCodeIndex;
GA_color            defcolor,maxcolor,foreColor,backColor;
GA_modeInfo         modeInfo;
REF2D_driver        *ref2d;
ibool               unloadRef2d;
GA_devCtx           *dc;
GA_2DStateFuncs     drv_state2d,state2d;
GA_2DRenderFuncs    drv_draw2d,draw2d;
REF2D_driver        *ref2d1 = NULL,*ref2d4 = NULL,*ref2d8 = NULL;
REF2D_driver        *ref2d16 = NULL,*ref2d24 = NULL,*ref2d32 = NULL;
PE_MODULE           *hMod1 = NULL,*hMod4 = NULL,*hMod8 = NULL;
PE_MODULE           *hMod16 = NULL,*hMod24 = NULL,*hMod32 = NULL;
GA_2DStateFuncs     state2d1,state2d4,state2d8,state2d16,state2d24,state2d32;
GA_2DRenderFuncs    draw2d1,draw2d4,draw2d8,draw2d16,draw2d24,draw2d32;
GA_bufferFuncs      bufmgr;
GA_buf              *primaryBuf;
GA_buf              *activeBuf;
GA_buffer           smallBuf;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
GA_color rgbColorEx(
    uchar r,
    uchar g,
    uchar b,
    GA_pixelFormat *pf)
{
    return ((ulong)((r >> pf->RedAdjust)   & pf->RedMask)   << pf->RedPosition)
         | ((ulong)((g >> pf->GreenAdjust) & pf->GreenMask) << pf->GreenPosition)
         | ((ulong)((b >> pf->BlueAdjust)  & pf->BlueMask)  << pf->BluePosition);
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
GA_color rgbColor(
    uchar r,
    uchar g,
    uchar b)
{
    return rgbColorEx(r,g,b,&modeInfo.PixelFormat);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColorEx(
    int index,
    int bitsPerPixel,
    uint colorMask,
    GA_pixelFormat *pf)
{
    if (bitsPerPixel <= 8)
        return index & colorMask;
    return rgbColorEx(VGA8_defPal[index].Red,
                      VGA8_defPal[index].Green,
                      VGA8_defPal[index].Blue,
                      pf);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColor(
    int index)
{
    return realColorEx(index,modeInfo.BitsPerPixel,maxcolor,&modeInfo.PixelFormat);
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.

NOTE: This version padded all the reserved values with 1's instead of
      zeroes. This is only intended for conformance testing the new WHQL
      style source transparent blit tests.
****************************************************************************/
GA_color rgbColorPaddedEx(
    uchar r,
    uchar g,
    uchar b,
    GA_pixelFormat *pf)
{
    if (dc->WorkArounds & gaNoWHQLTransparentBlit)
        return rgbColorEx(r,g,b,pf);
    return ((ulong)((r >> pf->RedAdjust)   & pf->RedMask)   << pf->RedPosition)
         | ((ulong)((g >> pf->GreenAdjust) & pf->GreenMask) << pf->GreenPosition)
         | ((ulong)((b >> pf->BlueAdjust)  & pf->BlueMask)  << pf->BluePosition)
         | ((ulong)((0xFF >> pf->AlphaAdjust)  & pf->AlphaMask)  << pf->AlphaPosition);
}

/****************************************************************************
REMARKS:
Returns the value representing the color. The value is converted from 24 bit
RGB space into the appropriate color for the video mode.
****************************************************************************/
GA_color rgbColorPadded(
    uchar r,
    uchar g,
    uchar b)
{
    return rgbColorPaddedEx(r,g,b,&modeInfo.PixelFormat);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColorPaddedEx(
    int index,
    int bitsPerPixel,
    uint colorMask,
    GA_pixelFormat *pf)
{
    if (bitsPerPixel <= 8)
        return index & colorMask;
    return rgbColorPaddedEx(VGA8_defPal[index].Red,
                            VGA8_defPal[index].Green,
                            VGA8_defPal[index].Blue,
                            pf);
}

/****************************************************************************
REMARKS:
Returns the real color value for the specified color. In RGB modes we look
up the color value from the palette and pack into the proper format.
****************************************************************************/
GA_color realColorPadded(
    int index)
{
    return realColorPaddedEx(index,modeInfo.BitsPerPixel,maxcolor,&modeInfo.PixelFormat);
}

/****************************************************************************
REMARKS:
Set the current foreground color for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetForeColor(
    GA_color color)
{
    state2d.SetForeColor(color);
    foreColor = color;
}

/****************************************************************************
REMARKS:
Set the current background color for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetBackColor(
    GA_color color)
{
    state2d.SetBackColor(color);
    backColor = color;
}

/****************************************************************************
REMARKS:
Set the current mix for both the hardware driver and the reference rasteriser.
****************************************************************************/
void SetMix(
    N_int32 mix)
{
    state2d.SetMix(mix);
}

/****************************************************************************
REMARKS:
Set the 8x8 monochrome bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Set8x8MonoPattern(
    N_int32 index,
    GA_pattern *pattern)
{
    state2d.Set8x8MonoPattern(index,pattern);
}

/****************************************************************************
REMARKS:
Select an 8x8 monochrome bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8MonoPattern(
    N_int32 index)
{
    state2d.Use8x8MonoPattern(index);
}

/****************************************************************************
REMARKS:
Select an 8x8 monochrome bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8TransMonoPattern(
    N_int32 index)
{
    state2d.Use8x8TransMonoPattern(index);
}

/****************************************************************************
REMARKS:
Set the 8x8 color bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Set8x8ColorPattern(
    N_int32 index,
    GA_colorPattern *pattern)
{
    state2d.Set8x8ColorPattern(index,pattern);
}

/****************************************************************************
REMARKS:
Select an 8x8 color bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8ColorPattern(
    N_int32 index)
{
    state2d.Use8x8ColorPattern(index);
}

/****************************************************************************
REMARKS:
Select an 8x8 color bitmap pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void Use8x8TransColorPattern(
    N_int32 index,
    GA_color transparent)
{
    state2d.Use8x8TransColorPattern(index,transparent);
}

/****************************************************************************
REMARKS:
Set the line stipple pattern for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetLineStipple(
    GA_stipple stipple)
{
    state2d.SetLineStipple(stipple);
}

/****************************************************************************
REMARKS:
Set the line stipple count for both the hardware driver and the
reference rasteriser.
****************************************************************************/
void SetLineStippleCount(
    N_uint32 count)
{
    state2d.SetLineStippleCount(count);
}

/****************************************************************************
REMARKS:
Set the plane mask for both the hardware driver and the reference rasteriser.
****************************************************************************/
void SetPlaneMask(N_uint32 planeMask)
{
    state2d.SetPlaneMask(planeMask);
}

/****************************************************************************
REMARKS:
Set the currently active drawing page for both the hardware driver and
the reference rasteriser. We use this to implement hardware page flipping.
****************************************************************************/
void SetActivePage(
    int page)
{
    activePage = page;
    if ((activeBuf = bufmgr.GetFlippableBuffer(page)) == NULL)
        PM_fatalError("Attempt to draw to invalid page!\n");
    bufmgr.SetActiveBuffer(activeBuf);
}

/****************************************************************************
REMARKS:
Set the currently visible page for the hardware driver. We use this to
implement hardware page flipping.
****************************************************************************/
void SetVisualPage(
    int page,
    int waitVRT)
{
    int     oldActivePage = activePage;
    GA_buf  *buf;

    visualPage = page;

    /* Flip to the visible buffer */
     
    buf = bufmgr.GetFlippableBuffer(visualPage);
    if (!buf)
        PM_fatalError("Attempt to flip to invalid page!\n");
    bufmgr.FlipToBuffer(buf,waitVRT);
}

/****************************************************************************
REMARKS:
Clears the current display page.
****************************************************************************/
void ClearPage(
    GA_color color)
{
    SetForeColor(color);
    draw2d.DrawRect(0,0,maxX+1,maxY+1);
}

/****************************************************************************
REMARKS:
Writes the string to the display
****************************************************************************/
void WriteText(
    int x,
    int y,
    char *s,
    GA_color color)
{
    static  uchar *monobuf = NULL;
    static  int bufsize = 0;
    uchar   *font,*image,*buf,*p;
    int     i,length,width,byteWidth,charsize = 16;

    /* Allocate a block of memory for building the mono string to blit */
    length = strlen(s);
    if (bufsize < length * charsize) {
        free(monobuf);
        bufsize = length * charsize;
        if ((monobuf = malloc(bufsize)) == NULL)
            PM_fatalError("Out of memory!");
        }

    /* Now build up the single mono blit string in the buffer */
    SetForeColor(color);
    font = dc->TextFont8x16;
    buf = monobuf;
    byteWidth = length;
    width = length * 8;
    while (length--) {
        p = buf++;
        image = (font + (*s++) * 16);
        for (i = 0; i < 16; i++) {
            *p = *image++;
            p += byteWidth;
            }
        }

    /* Finally draw the entire string as a single mono blit, clipped
     * if necessary
     */
    if (x <= 0 || y <= 0 || x+width >= modeInfo.XResolution || y+charsize >= modeInfo.YResolution) {
        draw2d.ClipMonoImageMSBSys(x,y,width,charsize,byteWidth,monobuf,true,
            0,0,modeInfo.XResolution,modeInfo.YResolution);
        }
    else {
        draw2d.PutMonoImageMSBSys(x,y,width,charsize,byteWidth,monobuf,true);
        }
}

/****************************************************************************
PARAMETERS:
x,y - Location to move text cursor to

REMARKS:
Moves the current text location to the specified position.
****************************************************************************/
void gmoveto(
    int _x,
    int _y)
{
    x = _x;
    y = _y;
}

/****************************************************************************
REMARKS:
Moves the current text location to the next line down
****************************************************************************/
void gnewline(void)
{ y += 16; }

/****************************************************************************
REMARKS:
Returns the current x position
****************************************************************************/
int ggetx(void)
{ return x; }

/****************************************************************************
REMARKS:
Returns the current y position
****************************************************************************/
int ggety(void)
{ return y; }

/****************************************************************************
PARAMETERS:
fmt     - Format string
...     - Standard printf style parameters

REMARKS:
Simple printf style output routine for sending text to the current graphics
modes. It begins drawing the string at the current location, and moves to
the start of the next logical line.
****************************************************************************/
int gprintf(
    char *fmt,
    ...)
{
    va_list argptr;
    char    buf[255];
    int     cnt;

    va_start(argptr,fmt);
    cnt = vsprintf(buf,fmt,argptr);
    WriteText(x,y,buf,defcolor);
    y += 16;
    va_end(argptr);
    (void)argptr;
    return cnt;
}

/****************************************************************************
REMARKS:
Find the draw buffer for the active drawing page in hardware video memory.
****************************************************************************/
static void GetActiveDrawBuffer(
    GA_buffer *drawBuf)
{
    if (smallBuf.dwSize != 0) {
        drawBuf->dwSize = sizeof(GA_buffer);
        drawBuf->Offset = smallBuf.Offset;
        drawBuf->Stride = smallBuf.Stride;
        drawBuf->Width = smallBuf.Width;
        drawBuf->Height = smallBuf.Height;
        }
    else {
        drawBuf->dwSize = sizeof(GA_buffer);
        drawBuf->Offset = activeBuf->Offset;
        drawBuf->Stride = activeBuf->Stride;
        drawBuf->Width = activeBuf->Width;
        drawBuf->Height = activeBuf->Height;
        }
}

/****************************************************************************
REMARKS:
This function forces the SciTech SNAP Graphics reference rasteriser functions to be
used for all drawing functions, overiding any hardware functions.
****************************************************************************/
void ForceSoftwareOnly(void)
{
    GA_buffer drawBuf;
    GetActiveDrawBuffer(&drawBuf);
    if (ref2d->SetDrawBuffer(&drawBuf,dc->LinearMem,modeInfo.BitsPerPixel,&modeInfo.PixelFormat,dc,true) != 0)
        PM_fatalError("REF2D_SetDrawBuffer failed!");
    forceSoftwareCount++;
}

/****************************************************************************
REMARKS:
Main function to initialise the software rasteriser module and the internal
library helper functions.
****************************************************************************/
void RevertSoftwareOnly(void)
{
    GA_buffer drawBuf;
    GetActiveDrawBuffer(&drawBuf);
    if (forceSoftwareCount)
        forceSoftwareCount--;
    if (forceSoftwareCount) {
        if (ref2d->SetDrawBuffer(&drawBuf,dc->LinearMem,modeInfo.BitsPerPixel,&modeInfo.PixelFormat,dc,true) != 0)
            PM_fatalError("REF2D_SetDrawBuffer failed!");
        }
    else {
        if (ref2d->SetDrawBuffer(&drawBuf,dc->LinearMem,modeInfo.BitsPerPixel,&modeInfo.PixelFormat,dc,softwareOnly) != 0)
            PM_fatalError("REF2D_SetDrawBuffer failed!");
        }
}

/****************************************************************************
REMARKS:
Main function to initialise the software rasteriser module and the internal
library helper functions.
****************************************************************************/
ibool InitSoftwareRasterizer(
    int deviceIndex,
    int reservePages,
    ibool stereo)
{
    GA_initFuncs        init;
    GA_softStereoFuncs  stereoFuncs = {
        sizeof(stereoFuncs),
        GA_softStereoInit,
        GA_softStereoOn,
        GA_softStereoScheduleFlip,
        GA_softStereoGetFlipStatus,
        GA_softStereoWaitTillFlipped,
        GA_softStereoOff,
        GA_softStereoExit,
        };

    /* Initialise the dimensions for the screen and setup drawing buffer */
    maxX = virtualX-1;
    maxY = virtualY-1;
    maxPage = modeInfo.MaxBuffers-1;
    activePage = 0;
    isStereo = gaNoStereo;
    useBlueCode = true;
    blueCodeIndex = 255;

    /* Load the SciTech SNAP Graphics reference rasteriser for the color depth */
    memset(&state2d,0,sizeof(state2d));
    memset(&draw2d,0,sizeof(draw2d));
    memset(&bufmgr,0,sizeof(bufmgr));
    unloadRef2d = false;
    if ((ref2d = GA_getCurrentRef2d(dc->DeviceIndex)) == NULL) {
        modeInfo.dwSize = sizeof(modeInfo);
        init.dwSize = sizeof(init);
        GA_queryFunctions(dc,GA_GET_INITFUNCS,&init);
        init.GetCurrentVideoModeInfo(&modeInfo);
        unloadRef2d = true;
        if (!GA_loadRef2d(dc,false,&modeInfo,-1,&ref2d))
            return false;
        }
    drv_state2d.dwSize = sizeof(drv_state2d);
    GA_queryFunctions(dc,GA_GET_2DSTATEFUNCS,&drv_state2d);
    drv_draw2d.dwSize = sizeof(drv_draw2d);
    GA_queryFunctions(dc,GA_GET_2DRENDERFUNCS,&drv_draw2d);
    driver.dwSize = sizeof(driver);
    REF2D_queryFunctions(ref2d,GA_GET_DRIVERFUNCS,&driver);
    state2d.dwSize = sizeof(state2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DSTATEFUNCS,&state2d);
    draw2d.dwSize = sizeof(draw2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DRENDERFUNCS,&draw2d);

    /* Initialise the buffer manager and find the primary buffer */
    bufmgr.dwSize = sizeof(bufmgr);
    if (!REF2D_queryFunctions(ref2d,GA_GET_BUFFERFUNCS,&bufmgr))
        return false;
    if (!bufmgr.InitBuffers(reservePages,0,stereo ? &stereoFuncs : NULL))
        return false;
    activeBuf = primaryBuf = bufmgr.GetPrimaryBuffer();

    /* Setup for hardware rendering to the framebuffer */
    forceSoftwareCount = 0;
    RevertSoftwareOnly();

    /* Now initialise color depth specific values */
    switch (modeInfo.BitsPerPixel) {
        case 1:
            bytesPerPixel = 1;
            defcolor = maxcolor = 0x1;
            ref2d1 = ref2d;
            break;
        case 4:
            bytesPerPixel = 1;
            defcolor = maxcolor = 0xF;
            ref2d4 = ref2d;
            break;
        case 8:
            bytesPerPixel = 1;
            defcolor = 0xF;
            maxcolor = 0xFF;
            ref2d8 = ref2d;
            break;
        case 15:
        case 16:
            bytesPerPixel = 2;
            defcolor = maxcolor = rgbColor(0xFF,0xFF,0xFF);
            ref2d16 = ref2d;
            break;
        case 24:
            bytesPerPixel = 3;
            defcolor = maxcolor = rgbColor(0xFF,0xFF,0xFF);
            ref2d24 = ref2d;
            break;
        case 32:
            bytesPerPixel = 4;
            defcolor = maxcolor = rgbColor(0xFF,0xFF,0xFF);
            ref2d32 = ref2d;
            break;
        }

    /* Set the default mixes and state... */
    SetActivePage(0);
    SetVisualPage(0,gaDontWait);
    SetMix(GA_REPLACE_MIX);
    SetForeColor(defcolor);
    SetBackColor(0);

    /* Program the default color palette */
    if (modeInfo.BitsPerPixel == 8)
        driver.SetPaletteData(VGA8_defPal,256,0,false);
    else if (modeInfo.BitsPerPixel == 4)
        driver.SetPaletteData(VGA4_defPal,16,0,false);
    (void)deviceIndex;
    return true;
}

/****************************************************************************
REMARKS:
Function to load a software rasteriser module for a specific color depth.
This allows us to load different software rasteriser modules for different
color depths
****************************************************************************/
void LoadSysMemDriver(
    int bitsPerPixel)
{
    REF2D_driver        **ref2d = NULL;
    PE_MODULE           **hMod = NULL;
    ulong               size;
    GA_2DStateFuncs     *state2d = NULL;
    GA_2DRenderFuncs    *draw2d = NULL;

    /* Find the correct variable to store the rasteriser into */
    switch (bitsPerPixel) {
        case 1:
            ref2d = &ref2d1;
            hMod = &hMod1;
            state2d = &state2d1;
            draw2d = &draw2d1;
            break;
        case 4:
            ref2d = &ref2d4;
            hMod = &hMod4;
            state2d = &state2d4;
            draw2d = &draw2d4;
            break;
        case 8:
            ref2d = &ref2d8;
            hMod = &hMod8;
            state2d = &state2d8;
            draw2d = &draw2d8;
            break;
        case 15:
        case 16:
            ref2d = &ref2d16;
            hMod = &hMod16;
            state2d = &state2d16;
            draw2d = &draw2d16;
            break;
        case 24:
            ref2d = &ref2d24;
            hMod = &hMod24;
            state2d = &state2d24;
            draw2d = &draw2d24;
            break;
        case 32:
            ref2d = &ref2d32;
            hMod = &hMod32;
            state2d = &state2d32;
            draw2d = &draw2d32;
            break;
        }
    if (*ref2d == NULL) {
        if (!REF2D_loadDriver(NULL,bitsPerPixel,false,ref2d,hMod,&size))
            PM_fatalError("Unable to load reference rasteriser!\n");
        }
    state2d->dwSize = sizeof(*state2d);
    REF2D_queryFunctions(*ref2d,GA_GET_2DSTATEFUNCS,state2d);
    draw2d->dwSize = sizeof(*draw2d);
    REF2D_queryFunctions(*ref2d,GA_GET_2DRENDERFUNCS,draw2d);
}

/****************************************************************************
REMARKS:
Function to exit the software rasteriser module and clean up
****************************************************************************/
void ExitSoftwareRasterizer(void)
{
    if (ref2d1 && hMod1) REF2D_unloadDriver(ref2d1,hMod1);
    if (ref2d4 && hMod4) REF2D_unloadDriver(ref2d1,hMod4);
    if (ref2d8 && hMod8) REF2D_unloadDriver(ref2d1,hMod8);
    if (ref2d16 && hMod16) REF2D_unloadDriver(ref2d1,hMod16);
    if (ref2d24 && hMod24) REF2D_unloadDriver(ref2d1,hMod24);
    if (ref2d32 && hMod32) REF2D_unloadDriver(ref2d1,hMod32);
    if (unloadRef2d)
        GA_unloadRef2d(dc);
    ref2d1 = ref2d4 = ref2d8 = ref2d16 = ref2d24 = ref2d32 = NULL;
    hMod1 = hMod4 = hMod8 = hMod16 = hMod24 = hMod32 = NULL;
}

