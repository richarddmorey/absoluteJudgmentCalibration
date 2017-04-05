
/* 
 * This is a top-level test of the new movie library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <alfont.h>

#include "response.h"
#include "movieobj.h"
#include "video.h"

#include <mem.h>
#include "savemem.txt"

#define NUM_RESPONSES 3

#define COL_BLACK  0
#define COL_RED1   1
#define COL_RED2   2
#define COL_GREEN1 3
#define COL_GREEN2 4
#define COL_BLUE1  5
#define COL_BLUE2  6
#define COL_WHITE 16

extern ALFONT_FONT *fontlst[];
extern int UseDIO24;
int main()
{
  image *fr0,*fr1,*fr2,*fr3,*fr4;
  movie *mymovie;
  response *data;
  int i;
  int color;
  unsigned char colors[768] = 
    {16,   0,   0,    /*   1 */                      
     31,   0,   0,    /*   2 */
       0, 16,   0,    /*   3 */
       0, 31,   0,    /*   4 */
       0,   0, 16,    /*   5 */
       0,   0, 31,    /*   6 */
       0,   0,   0,    /*   7 */
       0,   0,   0,    /*   8 */
       0,   0,   0,    /*   9 */
       0,   0,   0,    /*  10 */
       0,   0,   0,    /*  11 */
       0,   0,   0,    /*  12 */
       0,   0,   0,    /*  13 */
       0,   0,   0,    /*  14 */
       0,   0,   0,    /*  15 */
     15, 31, 31    /*  16 */
    };

                        

  UseDIO24 = 0;
  /* must be called to intialize the Movie package */
  setup(640, 400, 500, 300, 15+32*(15 +32*15), 75, 15, "fonts.lst");
  makePalette(CLEAR);

  for (i=0; i<16; i++){
    color = colors[3*i+2] +32*(colors[3*i+1] +32*colors[3*i]);
    if(addColor(color) != i+1) error("Can't Build palette\n");
  }

  if(alfont_set_font_size(fontlst[1], 20)==ALFONT_ERROR) error("Can not set font size\n");  

    
  

  fr0  = newImage();
  fillPicBuf(Palette[COL_WHITE]);
  uploadImage(fr0);

  fr1  = newImage();
  fillPicBuf(Palette[COL_WHITE]);
  circlefill(picBuf  ,160,240,60, Palette[COL_RED1]);
  drawText(&picBuf, "-",154,236, 1, Palette[COL_WHITE]);
  circlefill(picBuf  ,480,240,60, Palette[COL_BLUE1]);
  drawText(&picBuf, "B",474,236, 1, Palette[COL_WHITE]);
  uploadImage(fr1);

  fr2  = newImage();
  fillPicBuf(Palette[COL_WHITE]);
  circlefill(picBuf  ,160,240,60, Palette[COL_GREEN1]);
  drawText(&picBuf, "!",154,236, 1, Palette[COL_WHITE]);
  circlefill(picBuf  ,480,240,60, Palette[COL_BLUE1]);
  drawText(&picBuf, "B",474,236, 1, Palette[COL_WHITE]);
  uploadImage(fr2);


 /* set up the movie, and run it */
  mymovie = newMovie(100);
  for(i = 0;i < 100; ){
    setMovie(mymovie, i, fr1, 1);
    i++;
    setMovie(mymovie, i, fr2, 1);
    i++;
  }
  clearScreen(15+32*(15+32*15));
  sleep(5);
/*  data = runMovie(mymovie, FULL_SHOW, 2); */
  clearScreen(0);  
  /* must be called when done with Movie package */
  cleanup(); 

  /* print out results */
  printf ("Number\tResponse\tTime\n");
/*  for (i=0; i<2; i++)
    printf ("%d\t'%c' (%x)\t%d\n", i, (data->x[i].resp)&0x00ff, data->x[i].resp, 
	    data->x[i].rt); */
  return 0;
}
