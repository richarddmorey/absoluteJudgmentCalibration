
/* 
 * This is a top-level test of the new movie library.
 */

#include <stdio.h>
#include <stdlib.h>

#include "movieobj.h"
#include "video.h"

#include <mem.h>

#define NUM_RESPONSES 3

int main()
{
  image *hello, *goodbye;
  movie *mymovie;
  response *data;
  int i;

  /* must be called to intialize the Movie package */
  SetupMoviePackage("myfonts");
  makePalette(GRAYSCALE);


#define CREATE  
#ifdef CREATE
  makePalette(GRAYSCALE);
  

  hello  = newImage();
  downloadImage(hello);
  fillPicBuf(128);
  drawText("hello", 160, 100, 1, 255);
  uploadImage(hello);

  goodbye  = newImage();
  downloadImage(goodbye);
  fillPicBuf(255);
  drawText("goodbye", 160, 100, 1, 0);
  uploadImage(goodbye);

  saveImage(hello, "hello.ppm", PPM);
  saveImage(goodbye, "goodbye.ppm", PPM);
#else
  hello   = loadImage("hello.ppm", PPM);
  goodbye = loadImage("goodbye.ppm", PPM);
#endif

  /* set up the movie, and run it */
  mymovie = initMovie(2);
  setMovie(mymovie, 0, hello,   100);
  setMovie(mymovie, 1, goodbye,   100);
  clearScreen(240);
  data = runMovie(mymovie, UNTIL_RESPONSE, NUM_RESPONSES);
  
  /* must be called when done with Movie package */
  CleanupMoviePackage(); 

  /* print out results */
  printf ("Number\tResponse\tTime\n");
  for (i=0; i<NUM_RESPONSES; i++)
    printf ("%d\t'%c' (%d)\t%d\n", i, data->x[i].resp, data->x[i].resp, 
	    data->x[i].rt);
  return 0;
}
