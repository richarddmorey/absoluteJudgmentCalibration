/****************************************************************************
*
*
*  
*  ========================================================================
*
* Language:     ANSI C
*
* Description:  
* 
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "snap/gasdk.h"

#define VenIDDIO24 0x1307
#define DevIDDIO24 0x0028

void allegro_exit(void);
/*---------------------------- Global Variables ---------------------------*/

GA_initFuncs    init;
GA_driverFuncs  driver;
PM_HWND         hwndConsole;
void            *stateBuf;
int             oldMode = -1;
int             xRes,yRes,bpp,refr;
int		apage, vpage;
int             SNAPon = 0;

int             UseDIO24 = 1;
short           DIO24r1 = 0;
short           DIO24r2 = 0;
int             DIO24no = 0;

static int      NumPCI = -1;
static PCIDeviceInfo    *PCI = 0;
static int              *DIO24Index = 0;
static int              NumDIO24 = 0;
static int              DIO24initStat;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
void PMAPI FatalErrorCleanup(void)
{
    if (oldMode != -1) {
        virtualX = virtualY = bytesPerLine = -1;
        init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);
        }
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    allegro_exit();
}

#if (0)
/****************************************************************************
REMARKS:
Display a list of available resolutions
****************************************************************************/
void AvailableModes(void)
{
    N_uint16    *modes;

    printf("Available graphics modes:\n");
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        printf("    %4d x %4d %d bits per pixel\n",
            modeInfo.XResolution, modeInfo.YResolution, modeInfo.BitsPerPixel);
        }
    printf("\nUsage: hello <xres> <yres> <bits>\n");
}

#endif

int initDIO24(int Ind)
{
    int reg1;
    int reg2;
    int val;

    if (Ind >= DIO24no) return (-1);
    reg1 = (PCI[DIO24Index[Ind]].u.type0.BaseAddress14 & 0xfffffffc) + 0x4c;
    reg2 = PCI[DIO24Index[Ind]].u.type0.BaseAddress18 & 0xfffffffc;
    val = PM_inpd(reg1);
    DIO24initStat = val;
    val &= 0xffffffbe;
    PM_outpd(reg1,val);
    DIO24r1 = reg1;
    DIO24r2 = reg2;
    return (0);
}

static int PCI_enumerateDevices(void)
{
    int             i;

    // If this is the first time we have been called, enumerate all
    // devices on the PCI bus.
    if (NumPCI == -1) {
        if ((NumPCI = PCI_getNumDevices()) == 0)
            return -1;
        PCI = malloc(NumPCI * sizeof(PCI[0]));
        DIO24Index = malloc(NumPCI * sizeof(DIO24Index[0]));
	
	if (!DIO24Index)
            return -1;
        for (i = 0; i < NumPCI; i++)
            PCI[i].dwSize = sizeof(PCI[i]);
        if (PCI_enumerate(PCI) == 0)
            return -1;
        // Build a list of PCI DIO24 boards
        for (i = 0, NumDIO24 = 0; i < NumPCI; i++) {
//          printf("%04X %04X ", PCI[i].VendorID,PCI[i].DeviceID);
            if((PCI[i].VendorID == VenIDDIO24) && (PCI[i].DeviceID == DevIDDIO24)){
               DIO24Index[NumDIO24++] = i;
            }
        }
        if (NumDIO24 == 0) return -1;
	DIO24no = NumDIO24;

//        TestDIO24(0);

    }
    return DIO24no;
}


/****************************************************************************
REMARKS:
Initialise the graphics mode with the specified resolution and
color depth.
****************************************************************************/
int InitGraphics(
    int x,
    int y,
    int bits)
{
    int         oldMode;
    N_uint16    *modes;

    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if (modeInfo.XResolution == x && modeInfo.YResolution == y && modeInfo.BitsPerPixel == bits) {
            cntMode = *modes;
            if (modeInfo.Attributes & gaHaveLinearBuffer)
                cntMode |= gaLinearBuffer;
            oldMode = init.GetVideoMode();
            virtualX = virtualY = bytesPerLine = -1;
            if (init.SetVideoMode(cntMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL) != 0)
                return -1;
            if (!InitSoftwareRasterizer(0,2,false))
                PM_fatalError("Unable to initialise software rasteriser!");
            return oldMode;
            }
        }
    return -1;
}

/****************************************************************************
****************************************************************************/
int initSNAP()
{
    if (SNAPon) return (0); /* already initialized */ 
    /* Check to see if we are running in a window */
    if (PM_runningInAWindow()) {
        printf("This program cannot run in a window. Please switch to a fullscreen mode.\n");
        return -1;
        }

    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,640,480,8,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Load the device driver for this device */
    if ((dc = GA_loadDriver(0,false)) == NULL)
        PM_fatalError(GA_errorMsg(GA_status()));
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver))
        PM_fatalError("Unable to get device driver functions!");
    if (UseDIO24){
        if (PCI_enumerateDevices() <0)
            PM_fatalError("Unable to enumerate PCI devices!");
        if (DIO24no > 0) initDIO24(0);
	else if (UseDIO24 > 0)
		PM_fatalError("Unable to find DIO24 board!");
    }
    SNAPon = 1;
    return 0;
}

int initGraph(int XX, int YY, int RefrR, int dpth)
{
    int dlt;
    xRes = XX;
    yRes = YY;
    if (dpth == 0)bpp = 8;
    else bpp = dpth;

    init.SetGlobalRefresh(RefrR);

    if ((oldMode = InitGraphics(xRes,yRes,bpp)) != -1) {
	refr = init.GetCurrentRefreshRate(); /* in 0.01sec */
	dlt = refr -100*RefrR;
        if ((dlt > 100)||(dlt < -100)){
	   PM_fatalError("Unable to set required refresh rate\n");
	}
        if (maxPage == 0){
           PM_fatalError("Unable to create 2 video memory pages\n");
           return -1;
        }
        if (!driver.IsVSync){
           PM_fatalError("Unable to get VSync info\n");
           return -1;
        }
        vpage = 0;
        apage = 1;
        SetActivePage(apage);
        SetVisualPage(vpage, gaWaitVRT);
        return maxPage;
    }
    PM_fatalError("Unable to get required graphics mode\n");
    return -1;
}

void exitGraph()
{ 

    virtualX = virtualY = bytesPerLine = -1;
    init.SetVideoMode(oldMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);

    /* Restore the console */
    ExitSoftwareRasterizer();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
}

void exitSNAP()
{

    if(DIO24Index)
       free(DIO24Index);
    if(PCI)
       free(PCI);
    DIO24no = 0;
    NumPCI = -1;

    /* Unload the device driver */
    GA_unloadDriver(dc);
    SNAPon = 0;
    return ;
}

void bitBltMem(
		void *src,
		int sPitch,
		int sLeft,
		int sTop,
		int width,
		int hight,
		int dLeft,
		int dTop)
{
	draw2d.BitBltSys(src, sPitch, sLeft, sTop, width, hight, dLeft, dTop, GA_REPLACE_MIX, 0);
}

int Retrace()
{
	return driver.IsVSync();
}

void setActiveP(
		int page)
{
	SetActivePage(page);
        apage = page;
}

void setVisualP(
		int page,
		int wait)
{
	if(wait)SetVisualPage(page, gaWaitVRT);
        else SetVisualPage(page, gaDontWait);

        vpage = page;
}

void getP(
		void *pal,
		int num,
		int index)
{
	driver.GetPaletteData((GA_palette *)pal,num,index);
}

void setP(
		void * pal,
		int num,
		int index,
		int waitVRT)
{
	driver.SetPaletteData((GA_palette *)pal,num,index,waitVRT);
}
