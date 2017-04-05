#include "sp.h"

extern ALFONT_FONT *fontlst[];
extern long double rtSlope;
extern int errorTotal;
extern movie *std_m;
extern image *std_i;

#define backup 0
#define STATUS RUN

#define rate 120

#define ITEM 240
#define CALITEM_S 180
#define CALITEM_E 240
#define CALITEM (CALITEM_E-CALITEM_S)
#define CRITITEM 176    /* (ITEM-CALITEM) */


int durval[3]={5,7,9};
extern int UseDIO24;

main()
{
  /* outfile= sub,blk,trl,item,prime,primeo,targ,alt,dur,resp,RT,acc,err */
  
  char ***word, ***prac,critdurC,caloutput[60];
  myParameters p;
  movie *stim_m;
  image **a;
  int t,seed;
  response *data;
  int resp; float RT; int acc;
  logtype *log;
  FILE *outfile;

  int oi[ITEM],code[ITEM],calcorrect[3]={0,0,0},i;
  int prime,primeo,blk,critdur,alt;
  
  UseDIO24=0;
  log=(logtype *)malloc(1*sizeof(logtype));
  word=malloc_word(ITEM);
  load_word(ITEM,word,"..\\WORDLIST");
  prac=malloc_word(3);
  load_word(3,prac,"..\\PRACLIST");

  initLog(STATUS,log);
  seed=log->seed;
  outfile=fopen(log->outfn,"w");

  setup(640,480,rate,8,0);
  if(alfont_set_font_size(fontlst[1], 20)==ALFONT_ERROR)
    error("Cannot Set Font Size");
  makePalette(GRAYSCALE);
  
  stim_m=newMovie(6);
  a= (image **)malloc(6*sizeof(image *));
  for (t=0;t<6;t++) a[t]=newImage();
  
  p.primetime=24;
  p.vert=10;
  p.ahoriz=80;
  p.avert=80;
  p.blanktime=60;
  p.pretime=60;
  p.posttime=60;
  p.xc=319;
  p.yc=239;
  p.tooFast=.2;

  initTrials(stim_m,a,&p);
  
  /*Practice Items*/ 
 {
    for (i=0;i<3;i++)
      oi[i]=i;
    distribute(oi,i<3,&seed);

    for (i=0;i<3;i++)
      code[i]=i%16;
    distribute(code,3,&seed);
    
    runMovieText("READY?",p.xc,p.yc,1,255);
    for (i=0;i<3;i++)
      {
	p.duration=durval[code[i]%3];
	p.targ=code[i]%2;
	p.item=oi[i];
	prime=(code[i]/2)%2;
	primeo=(code[i]/4)%2;
	alt=(code[i]/8);
	p.prime1=2+2*prime+primeo;
	p.prime2=2+2*prime+(1-primeo);
	p.alt1=alt;
	p.alt2=1-alt;
	p.ans=(p.targ+alt)%2;
	runFCTrial(stim_m,a,&p,&data,prac,&resp,&RT);
	
      }
   
  }
/*calibration*/
  
  blk=0;
  {
    for (i=0;i<CALITEM;i++)
      oi[i]=i+CALITEM_S;
    distribute(oi,CALITEM,&seed);

    for (i=0;i<CALITEM;i++)
      code[i]=i%12;
    distribute(code,CALITEM,&seed);

    p.targ=0;
    runMovieText("READY?",p.xc,p.yc,1,255);
    for (i=0;i<CALITEM;i++)
      {
	p.item=oi[i];
	p.duration=durval[code[i]%3];
	primeo=(code[i]/3)%2;
	alt=(code[i]/6);
	p.prime1=4+primeo;
	p.prime2=4+(1-primeo);
	p.alt1=alt;
	p.alt2=1-alt;
	p.ans=(p.targ+alt)%2;
	runFCTrial(stim_m,a,&p,&data,word,&resp,&RT);
	if (p.ans == resp) 
	  {
	   calcorrect[code[i]%3]++;
	   acc=1;
	  } 
	else acc=0;
	fprintf(outfile,"%03d %02d %03d %03d %d %d %d %d %d %d %+f %d %d \n",
	    log->subjnum, blk, i, p.item, 1, primeo, p.targ, alt, p.duration,resp, RT,acc, errorTotal );
	
      }
  }
 
/*Get  Dur Value*/
  runMovieText("Thank You: Please Get Experimenter",p.xc,p.yc,1,255);
  sprintf(caloutput,"%d: %lf   %d: %lf   %d \%lf",
  durval[0],3.0*calcorrect[0]/CALITEM,durval[1],3.0*calcorrect[1]/CALITEM,
  durval[2],3.0*calcorrect[2]/CALITEM);
  clearImage(std_i);
  drawTextR(std_i,caloutput,0,0,1,255);
  setMovie(std_m,0,std_i,1);
  data = runMovie(std_m,UNTIL_RESPONSE,1);
  critdurC=(data->x[0].resp) & 0x00ff;
  critdur=atoi(&critdurC);
  if (critdur==0) critdur=11; 
  p.duration=critdur;
  

/*blocks 1 and 2*/
 
 for (blk=1; blk<3; blk++) 
 {
    for (i=0;i<CRITITEM;i++)
      oi[i]=i;
    distribute(oi,CRITITEM,&seed);

    for (i=0;i<CRITITEM;i++)
      code[i]=i%16;
    distribute(code,CRITITEM,&seed);

    runMovieText("READY?",p.xc,p.yc,1,255);
    for (i=0;i<CRITITEM;i++)
      {
	p.targ=code[i]%2;
	p.item=oi[i];
	prime=(code[i]/2)%2;
	primeo=(code[i]/4)%2;
	alt=(code[i]/8);
	p.prime1=2+2*prime+primeo;
	p.prime2=2+2*prime+(1-primeo);
	p.alt1=alt;
	p.alt2=1-alt;
	p.ans=(p.targ+alt)%2;
	runFCTrial(stim_m,a,&p,&data,word,&resp,&RT);
	if (p.ans == resp) acc=1;
	else acc=0;
	fprintf(outfile,"%03d %02d %03d %03d %d %d %d %d %d %d %+f %d %d \n",
	    log->subjnum, blk, i, p.item, prime, primeo, p.targ, alt, p.duration,resp, RT, acc, errorTotal );
      }
   
  }

 cleanup();
 fclose(outfile);
}

