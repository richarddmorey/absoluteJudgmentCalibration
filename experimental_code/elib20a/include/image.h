#ifndef _IMAGE_H
#define _IMAGE_H

/*
 * File: image.h
 * Desc: Image interface.
 *
 */


#include <mem.h>
#include <allegro.h>

#define NUM_IMAGES 128
/*
#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
*/
/* #define IMAGE_DEPTH  1 */
/* #define IMAGE_SIZE   307200; */

typedef BITMAP*  image;

void setPicSize(int pX, int pY);

/* Setup/destroy image data structures. */
void goImages(void);
void killImages(void);

/* Creates a new, blank, image. (Implicitly clears the picBuf). */
image *newImage(void);

/* The picture bufffer is a playground where the image can be directly
   manipulated. Use down/up to transfer playground data from/to image
   structures. The image should already have been created by new or load. */
extern BITMAP *picBuf;
void  copyImage(image *s, image *d);
void  downloadImage(image *im);
void  uploadImage(image *im);
#define clearPicBuf() clear_bitmap(picBuf);
#define fillPicBuf(x) clear_to_color(picBuf,(x))
#define clearImage(x) clear_bitmap(*(x))
#define fillImage(x,y) clear_to_color(*(x), (y))


/* To draw text in the picture buffer  */
void  drawText(image *im, const char *text, int x, int y, int font, int color);
void  drawTextL(image *im, const char *text, int x, int y, int font, int color);
void  drawTextR(image *im, const char *text, int x, int y, int font, int color);
void  drawText_(image *im, const char *text, int x, int y, int font, int color, int just);

/* File operations, load and save images. */
#define PPM  1
image *loadImage(const char *fname, int type);
void  saveImage(image *im, const char *fname, int type);

void encodeImage(image *im, int *palette, int max_col);

#endif /* _IMAGE_H */
