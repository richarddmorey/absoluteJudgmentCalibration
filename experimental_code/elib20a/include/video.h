#ifndef _VIDEO_H
#define _VIDEO_H

/* 
 * Direct access to graphics.
 */


#include <allegro.h>

void drawPicture(BITMAP *b, response *r);

void setPicSettings(BITMAP *picbuf, int w, int h, int x, int y, int depth);


/* the mode classes */
#define TEXT_MODE     0
#define GRAPHICS_MODE 1

void clearScreen  (int color);

void setDisplayParams(int width, int height, int refr, int depth);

void checkPicSettings(void);

void SetBG(int bg);

void setVideoMode (int mode);

void FlipPages(int wait);

int  Retrace(void);

void ClrPage(int color);

#define PIC_POS_X 0
#define PIC_POS_Y 0


#define palentry RGB

extern unsigned int Palette[256];
void setPalette(void);
void writePalette(const char *fname);
void readPalette(const char *fname);
int  paletteSize();

#define PALETTE_FULL -1
int  addColor(unsigned int p);

#define CLEAR     0
#define GRAYSCALE 1
#define EMPTY     2
void makePalette(int paltype);


#define NOT_FOUND -1
int findColor(unsigned int p);

#define MAX_COLORS 256
#define MAX_COLOR_INTENSITY 256

#endif /* _VIDEO_H */
