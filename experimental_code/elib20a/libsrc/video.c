/*
 * File: video.c
 * Desc: Low level video control.
 */

#include <allegro.h>
#include "response.h"
#include "graph.h"
#include "video.h"
#include "error.h"

#include <mem.h>

#include <stdio.h>

#include <conio.h>

#include "tsc_time.h"

unsigned int Palette[256];

static BITMAP *pb;
static int picWidth=-1;
static int picHeight=-1;
static int picDepth;
static int picX0=-1, picY0=-1, picBG=0;

static int currMode = TEXT_MODE;

static int currPalSize = 0;

extern int AllegroON;

static int gfx_refr = 90,
           gfx_w=640,
           gfx_h=480,
           gfx_dpth=16,
	   gfx_palette=0;

static BITMAP *gfx_bgline=0;

long long unsigned int time_stamp, time_0;

void setDisplayParams(int width, int height, int refr, int depth)
{
	if(width > 0) gfx_w = width;
	if(height > 0) gfx_h = height;
        if(refr > 0) gfx_refr = refr;
        if(depth > 0) gfx_dpth = depth;
}

void checkPicSettings()
{
        if((picWidth < 0)||(picWidth > gfx_w))  picWidth = gfx_w;
        if((picHeight < 0)||(picHeight > gfx_h)) picHeight = gfx_h;
        if((picX0 < 0)||(picX0 + picWidth > gfx_w)) picX0 = (gfx_w - picWidth)/2;
        if((picY0 < 0)||(picY0 + picHeight > gfx_h)) picY0 = (gfx_h - picHeight)/2;

}

void setPicSettings(BITMAP *picbuf, int width, int height, int x, int y, int depth)
{
  pb = picbuf;
  if(width > 0)picWidth  = width;
  if(height > 0)picHeight = height;
  if(x >= 0)picX0 = x;
  if(y>=0)picY0 = y;
  picDepth = depth;
}

void setPicBG(int bg)
{
	picBG = bg;
}

/*
 * Draw the picture in b to the screen. (Uses pb if b - NULL)
 */
void drawPicture(BITMAP * b, response * r)
{
  BITMAP *bb;
  int i;

  if (b) bb=b; else bb = pb;

/*  bitBltMem(bb->line[i], bb->w, 0, 0, bb->w, bb->h, 0, 0); */
  if (r){ 
     for(i = 0; i < bb->h; i++){ 
        bitBltMem(bb->line[i], bb->w, 0, 0, bb->w, 1, picX0, i+picY0);
        checkResponse(r);
     }
  }
  else{
     for(i = 0; i < bb->h; i++){
        bitBltMem(bb->line[i], bb->w, 0, 0, bb->w, 1, picX0, i+picY0);
     }
  }
}

/*
 * Function SetVideoMode
 * Changes to one of OUR modes (not SVGA).
 * As error() uses SetVideoMode(TEXT_MODE), don't use error() while setting TEXT_MODE 
 */
void setVideoMode(int mode)
{
  if(mode == TEXT_MODE){
    if (currMode == TEXT_MODE) return; /* ? */
    if (currMode == GRAPHICS_MODE){
      exitGraph();
      currMode = TEXT_MODE;
    }
  }else if(mode == GRAPHICS_MODE){
    if (currMode == GRAPHICS_MODE) return;
    checkPicSettings();
    /* Check if proper line buffer is allocated, if not - allocate */
    if (gfx_bgline && gfx_bgline->w != gfx_w){ 
       destroy_bitmap(gfx_bgline);
       gfx_bgline = 0;
    }
    if(gfx_bgline == 0)gfx_bgline = create_bitmap(gfx_w, 1);
    
    /* Set required graphics mode */
    gfx_palette = (gfx_dpth == 8? 1:0);
    initGraph(gfx_w, gfx_h, gfx_refr, gfx_dpth);
    currMode = GRAPHICS_MODE;
    ClrPage(picBG);
    FlipPages(0);  
    ClrPage(picBG);  
    time_0 = time_stamp = read_tsc(); /* Time stamp of switching to graphics mode */
  }
}

void ClrPage(int color)
{
        int i;
        if( gfx_bgline == 0) gfx_bgline = create_bitmap(gfx_w, 1);
        clear_to_color(gfx_bgline, color);
        for (i = 0; i < gfx_h; i++) {
            bitBltMem(gfx_bgline->line[0], gfx_w, 0, 0, gfx_w, 1, 0, i);
        }
}

/* 
 * Function: clearScreen()
 */
void clearScreen(int color)
{

  switch (currMode)
    {
    case TEXT_MODE:
      clrscr(); /* It looks in the original it ignored parameter "color" for text as well */
      break;
    case GRAPHICS_MODE:
      ClrPage(color);
      FlipPages(1);
      break;
    }
}

void FlipPages(int wait)
{   
    setVisualP(apage, wait);
    time_stamp = read_tsc();
    setActiveP(1-apage);
}


void setPalette(void)
{
  if (gfx_palette)
     setP(Palette,currPalSize,0,0);
}

void writePalette(const char *fname)
{
  FILE *f;
  int i, r, g, b;

  f = fopen(fname, "w");
  if (!f)
    error ("could not open file \"%s\"\n", fname);
  for (i=0; i<MAX_COLORS; i++){
    r = Palette[i];
    b = r & 0x000000ff;
    r = r>>8;
    g = r & 0x000000ff;
    r = r>>8;
    fprintf (f, "%d %d %d\n", r, g, b);
  }
  fclose(f);
}

void readPalette(const char *fname)
{
  FILE *f;
  int ret,i; 
  unsigned int r,g,b;

  f = fopen(fname, "r");
  if (!f)
    error ("could not open file \"%s\"\n", fname);
  for (i=0; i<MAX_COLORS; i++)
    {
      ret = fscanf (f, "%d %d %d\n", &r, &g, &b);
      if(ret != 3) error (" file \"%s\" has incorrect format\n", fname);
      Palette[i] =  b + (g<<8) +(r<<16) ;
    }
  fclose(f);
}

int  paletteSize()
{
  return currPalSize;
}

int addColor(unsigned int p)
{
  if (currPalSize < MAX_COLORS)
    {
      Palette[currPalSize] = p;
      return currPalSize++;
    }
  else
    return PALETTE_FULL;
}


int findColor(unsigned int p)
{
  int i;

  for (i=0; i<currPalSize; i++)
    if (Palette[i]==p) 
      return i;
  return NOT_FOUND;
}

void makePalette(int paltype)
{
  int i;

  switch (paltype)
    {
    case GRAYSCALE:
      for (i=0; i<MAX_COLORS; i++)
	Palette[i] = i*(1+256*(1+256));
      currPalSize = MAX_COLORS;
      break;
    case CLEAR:
      for (i=0; i<MAX_COLORS; i++)
	Palette[i] = 0;
      /* save the color black */
      currPalSize = 1;
      break;
    case EMPTY:
      currPalSize = 1;
      break;
    }
}
