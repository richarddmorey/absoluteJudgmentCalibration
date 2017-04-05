
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
extern int errorTotal, errorTerm;
int main()
{
  image *fr0,*fr1,*fr2,*fr3,*fr4;
  movie *mymovie;
  response *data;
  int i,j,k,c1,c2,c3;
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
  int pal[4096];
  FILE *coltable;                     

  UseDIO24 = 0;
  coltable = fopen("12bit","r");
  for(i=0; i<4096; i++){
    fscanf(coltable,"%d", &c1);  
    pal[i] = c1;
  }
  fclose(coltable);
  /* must be called to intialize the Movie package */
  setup(640, 480, 512, 256, 127+256*(127 +256*127), 120, 32, "fonts.lst");
  makePalette(CLEAR);

  for (i=0; i<16; i++){
    color = colors[3*i+2] +256*(colors[3*i+1] +256*colors[3*i]);
    if(addColor(color) != i+1) error("Can't Build palette\n");
  }

  if(alfont_set_font_size(fontlst[1], 20)==ALFONT_ERROR) error("Can not set font size\n");  

    
  

  fr0  = newImage();
  fillPicBuf(Palette[COL_WHITE]);
  for (i=0; i<512; i++){
    for(j=0; j<256; j++){
     putpixel(*fr0,i, j, pal[2000+i+j]); 
    /* ((int *) ((*fr0)->line[j]))[i] = (j + i*256); */
    }     
  }     
/*  uploadImage(fr0);

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

*/
 /* set up the movie, and run it */
  mymovie = newMovie(100);
  for(i = 0;i < 100; ){
    setMovie(mymovie, i, fr0, 10);
    i++;
  }
  clearScreen(127+256*(127+256*127));
  data = runMovie(mymovie, FULL_SHOW, 2);
  clearScreen(0);  
  /* must be called when done with Movie package */
  cleanup(); 

  /* print out results */
  printf ("Errors: %d \n", errorTotal);
  printf ("Number\tResponse\tTime\n");
  for (i=0; i<2; i++)
    printf ("%d\t'%c' (%x)\t%d\n", i, (data->x[i].resp)&0x00ff, data->x[i].resp, 
	    data->x[i].rt);
  return 0;
}
