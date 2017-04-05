
/*
 * Movie Implementation 
 * 
 */
#include "response.h"
#include "movieobj.h"
#include "video.h"
#include "input.h"
#include "error.h"
#include "tsc_time.h"
#include "graph.h"
#include "parport.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include <alfont.h>

void setPicSize(int pX, int Py);

static response *runMovieFull(movie *m, int numResponses);
static response *runMovieUntilResponse(movie *m, int numResponses);
int FrameTiming();
void defFont(char *pth, int mxlen);
extern long long unsigned int time_stamp;
extern long double tsc_Slope;
extern int DIOThr;

static inline void endShowProcess()
{
	while(refreshParPort());
}

int errorTerm = 0; /* Flag requesting termination on missing timing */
int errorTotal = 0; /* Total number of timing error */

int notFirst = 0;  /* Flag is set to 0 before running a movie - requests getting frame timing and initial time */
image * std_i = 0;
movie * std_m = 0;

static long long unsigned int lastFrameTime = 0;

static int frameDuration = 0, frameTol = 0;
static int pulseDuration = 0;

static response *r = 0;

int Refresh_rate = 90;
int AllegroON = 0;

int showStarted = 0;
long long unsigned int rtStarted = 0;

static id = 0;

ALFONT_FONT * fontlst[16]; 

void setup(int sX, int sY, int pX, int pY, int bg, int refr, int depth, const char *fontlistfname)
{
  FILE *fontlist;

  char fname[FILENAME_MAX];
  char deffnt[1040];
  int  fonttype;
  int pathset = 0;
  id = 1;
  if (depth == 0) {
       depth = 8;
     } else if (depth == 8){
     } else if ((depth == 15) || (depth == 16)) {
     } else if (depth == 24){
     } else if (depth == 32){
     } else error ("wrong color depth\n"); 

  GetTimePoint(5);

  if (initSNAP())
    error("can't initialize SNAP\n");

  if (allegro_init())
    error("can't initialize Allegro\n");
  AllegroON = 1;

  set_color_depth(depth); /* it's for Allegro only, not for SNAP */
  setDisplayParams(sX, sY, refr, depth);
  setPicSize(pX, pY);
  setPicBG(bg);

  alfont_init();

  if (fontlistfname == NULL) 
    {
      /* if no fontlist filename given, just try some defaults */
      defFont(deffnt,1024);
      strcat(deffnt, "verdana.ttf");
      if (!(fontlst[id] = alfont_load_font(deffnt)))
         error("default font not loaded\n");
      id++;
    }
  else 
    {
      if ((fontlist = fopen(fontlistfname, "r")) == NULL) 
	error("could not open font list file: \"%s\"", fontlistfname);
      
      /* default directory is current directory */
      while (!feof(fontlist))
	{
	  fname[0] = 0;
	  fgets(fname, FILENAME_MAX, fontlist);
	  /* chop off newline if present */
	  if (fname[strlen(fname)-1] == '\n')
	    fname[strlen(fname)-1] = 0;
	  /* if it's a blank line, just skip */
	  if (fname[0] == 0) 
	    continue;

	  if (id>15) error (" too many fonts in the list\n");    
	  if(!(fontlst[id] = alfont_load_font(fname)))
	     error("font %s not loaded\n", fname);
          else {
             id++;
          }
	      /* else printf ("loaded %s as id %d\n", fname, id); */
	}
    }

  goImages();

  GetTimeParams(); /* read and update tsc_Slope and tsc_T0 coeffs (timing) */
  DIOThr = 0.02/tsc_Slope ; 
  setVideoMode(GRAPHICS_MODE);

  if(std_i == 0)std_i = newImage();
  if(std_m == 0){
      std_m = newMovie(1);
      std_m->useParPort = 0;
  }

/*  makePalette(CLEAR); */
  install_keyboard();
  frameDuration = FrameTiming();
  frameTol = frameDuration >> 3;
  pulseDuration = frameDuration /4; 
 /* InitTimer(); */
}

void cleanup(void)
{
  killMovie(std_m);
  std_m = 0;
  std_i = 0;
  remove_keyboard();
  killImages();
  while(id >0){
    alfont_destroy_font(fontlst[id]);
    id--;
  }
  alfont_exit();  
  setVideoMode(TEXT_MODE);
}

movie *newMovie(int numImages)
{
  movie *m;
  int i;
  m = (movie *) malloc(sizeof(movie));
  m->numImages = numImages;
  m->useParPort = 0;
  m->x = (struct movie_image *) calloc(numImages, sizeof(struct movie_image));

  for(i=0;i<numImages;i++){
     m->x[i].ts = 0;
     m->x[i].im = 0;
     m->x[i].numFrames = 0;
     m->x[i].errorFlg = 0;
     m->x[i].parVal = 0;
  }    
  return m;
}

void killMovie(movie *m)
{
   if (m){
      if (m->x)free(m->x);
      free (m);
   }
}

void setParVal(movie *m, int pos, int parVal)
{
  m->x[pos].parVal = parVal;
}

void  setMovie(movie *m, int pos, image *im, int numFrames)
{
  m->x[pos].im = im;
  m->x[pos].numFrames = numFrames;
  m->x[pos].errorFlg = 0;
  m->x[pos].parVal = 0;
  m->x[pos].ts = 0;
}

void setMovieP(movie *m, int pos, image *im, int numFrames, int parVal)
{
  setMovie(m, pos, im, numFrames);
  setParVal(m, pos, parVal);
}

void encodeMovieM(movie *m, int *Palette, int max_col)
{
  int i;

  for (i=0; i<m->numImages; i++)
      encodeImage(m->x[i].im, Palette, max_col);

}

void encodeMovie(movie *m, int *Palette)
{
  encodeMovieM(m, Palette, 256);
}

response *runMovie(movie *m, int showType, int numResponses)
{
   notFirst = 0;
   return runMovieC(m, showType, numResponses);
}

response *runMovieC(movie *m, int showType, int numResponses)
{
  response *r;

  int i, f, dt, dt1, errFl = 0;
  int valPar = 0;
  int flgPar = 0;
  int flgNoWait = 0;

  flgNoWait = showType & END_NO_WAIT; 
  showType = showType & (~ END_NO_WAIT);
  
  setPalette();

  r = initResponse(numResponses);
  
  r->useParPort =(flgPar = (m->useParPort)); 

  /* get in sync with retrace */
  if (!notFirst){
    while (!Retrace());
    while (Retrace());  
    lastFrameTime = read_tsc(); 
    notFirst = 1;
    showStarted = 0;
    FlushKeyBuf();
    FlushDIO24();
  }
  if(m->useParPort){
     initParPort(pulseDuration);
     valPar = PP_MOVIE_STARTED;
  }
  for (i=0; i<m->numImages; i++){    
      if (showStarted) checkResponse(r);

      if(flgPar){
	 valPar |= m->x[i].parVal;
      }

      /* Draw  image # i - not visible yet */
      drawPicture (*(m->x[i].im),(showStarted ? r : 0)); /* until flip - previous frame on the screen */

      /* Show  number of frames requested */
      for (f=0; f < m->x[i].numFrames; f++){

          if (showStarted &&(showType == UNTIL_RESPONSE)&&(responseReady(r))){
          /* end of show - user responded */
            endShowProcess();
	    return r;
          }

          while(!Retrace())
            if(showStarted)checkResponse(r);
 
         /* We are retracing now */ 
          if (f == 0) FlipPages(0);

          while(Retrace()){
             if(showStarted)checkResponse(r);
	  }

          /* Now image i frame f+1 starts displaying - get timing */
          time_stamp = read_tsc(); /* time of end of retrace */

	  if(valPar){
	     outParPort(valPar);
	     valPar=0;
	  }
          dt = time_stamp - lastFrameTime;
          if (f == 0) m->x[i].ts = time_stamp;
          lastFrameTime = time_stamp;
	  if(!showStarted){
             showStarted = 1;
             rtStarted = lastFrameTime;
          }  
          dt1 = dt -= frameDuration;          
          if (dt <0 ) dt = -dt;
          if (dt>frameTol){
              /*error("Missed timing  %d %d\n", dt, frameDuration);*/
              if ((f > 0) && (f < m->x[i].numFrames - 1)){
                 dt1 -= frameDuration;
                 if(dt1 < 0) dt1 = -dt1;
                 if (dt1 < 2*frameTol) f++; else errFl = 1;
              } else errFl = 1;
          }
          if (errFl){
             errFl = 0;
             if(errorTerm){
		endShowProcess();
	        error("Missed timing in frame %d\n", i+1);
             }		
             else{
                 m->x[i].errorFlg++;
                 errorTotal++;
             }
          } 
      }
  }

  while (!(flgNoWait || responseReady(r)))
    ; 
  endShowProcess();
  return r;
}

/* estimates period of a frame in computer cycles (median of 9 cycles) */
int FrameTiming()
{
  long long unsigned int cur, last;
  int dt[10], i, j, t;

  for(i = -1; i < 9; i++){
    while (!Retrace());
    while (Retrace());
    cur = read_tsc();
    if(i >= 0) dt[i] = cur - last;
    last = cur;
  }
  for(i = 0; i<5; i++){
    for (j = 8; j>i; j--){
      if(dt[j] > dt[j-1]){
        t = dt[j];
        dt[j] = dt[j-1];
        dt[j-1] = t;
      }
    }
  }
  return dt[4];
}

void defFont(char * pth, int maxln)
{
        char *pt, *pt1;
       
        pt = getenv("SNAP_PATH");
        if(pt){
          if (strlen(pt) > maxln) error("SNAP_PATH too long\n");
          for(pt1=pth;;){
            if (!(*pt1 = ( *pt == '\\') ? '/' : *pt )) break;
            pt1++;
            pt++;
          }
          /* delete last slash */
          for(;;){
            pt1--;
            if ((pt1 == pth)|| (*pt1 != '/')) break;
            *pt1 = 0;
          }
          if (pth[0] == 0) 
             strcpy(pth, "c:/SNAP/fonts/");
          else{
             for(;;){
                if((pt1 == pth)||(*pt1 == '/')) break;
                pt1--;
             } 
             strcpy(pt1,"/fonts/");
          }      
        }
        else strcpy(pth,"c:/SNAP/fonts/"); 
	return ;
}
