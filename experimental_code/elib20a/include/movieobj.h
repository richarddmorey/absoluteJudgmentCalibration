#ifndef _MOVIE_H
#define _MOVIE_H

/*
 * Movie Library Interface
 *
 * The Movie Library provides two functionalities: 1) the movie object
 * and 2) direct access to the graphics screen. A movie object unites
 * responses and images.  
 */

#include "image.h"

/* The movie data structure. */
typedef struct {
  short int numImages;
  short int useParPort; /* 1 - not trigger par. port out */
  struct movie_image {
    long long unsigned int ts; /* time stamp */
    image *im;
    int numFrames;
    short int errorFlg;
    short int parVal; /* if 1, 2 or 3 - set corresponding bit on par. port */
  } *x;
} movie;

/*
 * Image/Movie Functions
 */

#define FULL_SHOW      (1)
#define UNTIL_RESPONSE (2)
#define END_NO_WAIT    (128)

movie *newMovie(int numImages);

void killMovie(movie *m);

void  setMovie(movie *m, int pos, image *im, int numFrames);

void setMovieP(movie *m, int pos, image *im, int numFrames, int parVal);

void setParVal(movie *m, int pos, int parVal);

response *runMovie(movie *m, int showType, int numResponses);

response *runMovieC(movie *m, int showType, int numResponses);

/* 
 * Administration
 */

void setup(int sX, int sY, int sX0, int sY0, int bg, int refr, int colord, const char *fontlistfname);

void cleanup(void);

void encodeMovieM(movie *m, int *Palette, int max_col);

void encodeMovie(movie *m, int *Palette);

#endif /* _MOVIE_H */
