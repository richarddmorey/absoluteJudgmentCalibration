
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
    {128,   0,   0,    /*   1 */                      
     255,   0,   0,    /*   2 */
       0, 128,   0,    /*   3 */
       0, 255,   0,    /*   4 */
       0,   0, 128,    /*   5 */
       0,   0, 255,    /*   6 */
       0,   0,   0,    /*   7 */
       0,   0,   0,    /*   8 */
       0,   0,   0,    /*   9 */
       0,   0,   0,    /*  10 */
       0,   0,   0,    /*  11 */
       0,   0,   0,    /*  12 */
       0,   0,   0,    /*  13 */
       0,   0,   0,    /*  14 */
       0,   0,   0,    /*  15 */
     255, 255, 255    /*  16 */
    };

                        

  UseDIO24 = 1;
  setDisplayParams(640,480,90);
  /* must be called to intialize the Movie package */
  setup(640, 480, 90, 8, "fonts.lst");
  makePalette(CLEAR);

  for (i=0; i<16; i++){
    color = colors[3*i+2] +256*(colors[3*i+1] +256*colors[3*i]);
    if(addColor(color) != i+1) error("Can't Build palette\n");
  }

  if(alfont_set_font_size(fontlst[1], 20)==ALFONT_ERROR) error("Can not set font size\n");  

    
  

  fr0  = newImage();
  fillPicBuf(COL_WHITE);
  uploadImage(fr0);

  fr1  = newImage();
  fillPicBuf(COL_WHITE);
  circlefill(picBuf  ,160,240,60, COL_RED1);
  drawText(&picBuf, "-",154,236, 1, COL_WHITE);
  circlefill(picBuf  ,480,240,60, COL_BLUE1);
  drawText(&picBuf, "B",474,236, 1, COL_WHITE);
  uploadImage(fr1);

  fr2  = newImage();
  fillPicBuf(COL_WHITE);
  circlefill(picBuf  ,160,240,60, COL_GREEN1);
  drawText(&picBuf, "!",154,236, 1, COL_WHITE);
  circlefill(picBuf  ,480,240,60, COL_BLUE1);
  drawText(&picBuf, "B",474,236, 1, COL_WHITE);
  uploadImage(fr2);


 /* set up the movie, and run it */
  mymovie = newMovie(450);
  for(i = 0;i < 450; ){
    setMovie(mymovie, i, fr1, 1);
    i++;
    setMovie(mymovie, i, fr2, 1);
    i++;
  }
  clearScreen(0);     
  data = runMovie(mymovie, FULL_SHOW, 10);
  clearScreen(0);  
  /* must be called when done with Movie package */
  cleanup(); 

  /* print out results */
  printf ("Number\tResponse\tTime\n");
  for (i=0; i<10; i++)
    printf ("%d\t'%c' (%x)\t%d\n", i, (data->x[i].resp)&0x00ff, data->x[i].resp, 
	    data->x[i].rt);
  return 0;
}
