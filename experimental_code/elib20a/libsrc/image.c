
/*
 * Image implementation.
 */

#include <allegro.h>
#include <alfont.h>
#include "image.h"
#include "response.h"
#include "video.h"
#include "error.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mem.h>

extern ALFONT_FONT *fontlst[];
static image IMAGE[NUM_IMAGES];
static int imgCounter=0;

BITMAP * picBuf;

int picWidth = 480, picHeight=640, picDepth=0;

static void loadPPM (FILE *f);
static void savePPM (FILE *f);

static char *ascii_r ="r";
static char *bin_r   ="r";
static char *ascii_w ="w";
static char *bin_w   ="w";

void setPicSize(int pX, int pY)
{
  picWidth = pX;
  picHeight = pY;
}

void goImages(void)
{
  int i;
  
  /* init the image handle array */
  for (i=0; i<NUM_IMAGES; i++)
    IMAGE[i] = 0;

  /* Set up the picture buffer. */
  picBuf = create_bitmap(picWidth, picHeight);
  if (picBuf == NULL)
    error ("no mem for picbuf\n");
  clear_bitmap(picBuf);  

  setPicSettings(picBuf, picWidth, picHeight, -1, -1, picDepth); 
}

void killImages(void)
{
  while(imgCounter > 0){
    imgCounter--;
    destroy_bitmap(IMAGE[imgCounter]);
    IMAGE[imgCounter] = 0; /* not really nesessary */
  }  
  destroy_bitmap(picBuf);
}

image *newImage(void)
{
  if (imgCounter == NUM_IMAGES)
    return 0;
  IMAGE[imgCounter] = create_bitmap(picWidth, picHeight);
  clear_bitmap(IMAGE[imgCounter]);   
  return &IMAGE[imgCounter++];
}

void copyImage(image *s, image *d)
{
  blit(*s, *d, 0, 0, 0, 0, picWidth, picHeight);
}

void downloadImage(image *im)
{
  blit(*im, picBuf, 0, 0, 0, 0, picWidth, picHeight);
}

void uploadImage(image *im)
{
  blit(picBuf, *im, 0, 0, 0, 0, picWidth, picHeight);
}

void  drawText(image *im,const char *text, int x, int y, int font, int color)
{
  drawText_(im, text, x, y, font, color, 1);
}

void  drawTextR(image *im,const char *text, int x, int y, int font, int color)
{
  drawText_(im, text, x, y, font, color, 0);
}

void  drawTextL(image *im,const char *text, int x, int y, int font, int color)
{
  drawText_(im, text, x, y, font, color, 2);
}

void  drawText_(image *im,const char *text, int x, int y, int font, int color, int just)
{
  image *img = &picBuf;
  if (im) img = im;
  alfont_text_mode(-1);
  if(just == 0)alfont_textout(*img, fontlst[font], text, x, y, color);
  else if (just == 1)alfont_textout_centre(*img, fontlst[font], text, x, y, color);
  else alfont_textout_right(*img, fontlst[font], text, x, y, color);
}

image *loadImage(const char *fname, int type)
{
  FILE *f;
  char *ftype;

  if (imgCounter == NUM_IMAGES)
    return 0;
  IMAGE[imgCounter] = create_bitmap(picWidth, picHeight);

  switch (type)
    {
    case PPM:
      ftype = ascii_r;
      break;
    }

  if ((f=fopen(fname, ftype))==NULL)
    clear_bitmap (picBuf);

  switch (type)
    {
    case PPM:
      loadPPM(f);
      break;
    }

  fclose(f);

  uploadImage(&IMAGE[imgCounter]);
  return &IMAGE[imgCounter++];
}

void saveImage(image *im, const char *fname, int type)
{
  FILE *f;
  char *ftype;

  downloadImage(im);

  switch (type)
    {
    case PPM:
      ftype = ascii_w;
      break;
    }
  
  f = fopen(fname, ftype);
  if (!f)
    error ("can't open file \"%s\"\n", fname);

  switch (type)
    {
    case PPM:
      savePPM(f);
      break;
    }

  fclose(f);
}

void savePPM(FILE *f)
{
  unsigned int i, j;
  int c;

  fprintf (f, "P3\n%d %d\n%d\n", picWidth, picHeight, MAX_COLOR_INTENSITY);
  for (i=0; i<picHeight; i++){
    for (j=0; j<picWidth; j++){
      c = getpixel(picBuf, j, i);      
      fprintf (f, "%s%d %d %d", j % 5 ? "   " : "\n", 
	     Palette[c] & 255, (Palette[c]<<8) & 255, Palette[c]<<16 & 255);
    }
  }
}

void loadPPM(FILE *f)
{
  unsigned pici;
  unsigned p;
  unsigned r, g, b;
  int width, height, maxcoli;
  char magic[10];
  int c;
  int i,j;

  /* read ppm header */
  fgets (magic, 10, f);
  magic[strlen(magic)-1] = 0;
  if (strcmp(magic, "P3") != 0)
    error ("ppm must be of type P3\n");
  fscanf (f, " %d %d %d", &width, &height, &maxcoli);
  if (width!=picWidth || height != picHeight)
    error ("ppm must be size %d by %d\n", picWidth, picHeight);
  if (maxcoli > MAX_COLOR_INTENSITY)
    error ("ppm must have intensity values below %d\n", MAX_COLOR_INTENSITY);
  
  pici = 0;
  for(i=0; i<picHeight; i++){
    for (j=0; j<picWidth; j++){
      if (feof(f)) error("PPM file corrupted\n");    
      /* read in the next pixel's color */
      fscanf (f, " %d %d %d", &r, &g, &b);
      p = r +256*(g + 256+b);

      if ((c=findColor(p)) == NOT_FOUND)
	{
	  if ((c=addColor(p)) == PALETTE_FULL)
	    error ("filled all %d palette slots while loading image\n", MAX_COLORS);
	}
      putpixel(picBuf, j, i, c);
    }
  }
}

void encodeImage(image *im, int * pal, int max_col)
{
  int x,y,xm=(*im)->w,ym=(*im)->h;
  int c;
  if (max_col <= 0) return;
  for(y=0;y<ym;y++){
    for(x=0;x<xm;x++){
      c = getpixel(*im,x,y);	    
      putpixel(*im,x,y,c<max_col ? pal[getpixel(*im,x,y)]:c);
    }
  }   
}
