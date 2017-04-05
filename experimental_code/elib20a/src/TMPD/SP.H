#include <allegro.h>
#include <alfont.h>
#include "elib.h"
#include "exptlib.h"
#include <stdlib.h>
#include <stdio.h>

#define TopChar 10

typedef struct
{
  int item,targ,alt1,alt2,prime1,prime2,ans;
  int pretime,posttime,blanktime;
  int duration,primetime;
  int vert,xc,yc,avert,ahoriz,tooFast;
} myParameters;

void initTrials(movie *stim_m,image **a,myParameters *p);


void runNameTrial(movie *stim_m, image **a, myParameters *p, response **data, char ***word);

void runFCTrial(movie *stim_m, image **a, myParameters *p, response **data,char ***word, int *resp, float *RT);

char *** malloc_word(int numI);
void load_word(int numI,char ***word, char *fname);


