#include <allegro.h>
#include <alfont.h>
#include "elib.h"
#include "exptlib.h"
#include <stdlib.h>
#include <stdio.h>


extern ALFONT_FONT *fontlst[];
extern long double rtSlope;
extern int errorTotal;
extern movie *std_m;
extern image *std_i;

#define backup 0
#define STATUS DEBUG

#define rate 60

extern int UseDIO24;

main()
{

  logtype *log;
  FILE *outfile;


  UseDIO24= 0;
  log=(logtype *)malloc(1*sizeof(logtype));
  initLog(STATUS,log);
  outfile=fopen(log->outfn,"w");

  setup(800,600,800,600, 0,rate,8,"\\fonts\\fonts.lst");
  if(alfont_set_font_size(fontlst[1], 18)==ALFONT_ERROR)
    error("Cannot Set Font Size");
  makePalette(GRAYSCALE);


  runMovieText("STOP!!! Go back to room 203",200,200,1,255);


 fclose(outfile);
 cleanup();
}
