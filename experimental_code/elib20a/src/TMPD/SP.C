#include "sp.h"
extern long double rtSlope;

void pad(char *dest, char *src, char *spacer);

void initTrials(movie *stim_m,image **a,myParameters *p)
{
  drawText(a[4],"$$$$$$$$$$",p->xc,p->yc,1,255);
  drawText(a[1],"$$$$$$$$$$",p->xc,p->yc+p->vert,1,255);
  drawText(a[1],"$$$$$$$$$$",p->xc,p->yc-p->vert,1,255);
  setMovie(stim_m,0,a[0],p->blanktime);
  setMovie(stim_m,1,a[1],p->pretime);
  setMovie(stim_m,4,a[4],p->posttime);
}


void runFCTrial(movie *stim_m, image **a, myParameters *p, response **data, char ***word,int *resp, float *RT)
{
  char targw[TopChar],alt1w[TopChar],alt2w[TopChar];
  char prime1w[TopChar],prime2w[TopChar];
  
  
  pad(targw,word[p->item][p->targ],"$");
  pad(alt1w,word[p->item][p->alt1]," ");
  pad(alt2w,word[p->item][p->alt2]," ");
  pad(prime1w,word[p->item][p->prime1],"$");
  pad(prime2w,word[p->item][p->prime2],"$");
  clearImage(a[2]);
  drawText(a[2],prime1w,p->xc,p->yc-p->vert,1,255);
  drawText(a[2],prime2w,p->xc,p->yc+p->vert,1,255);
  setMovie(stim_m,2,a[2],p->primetime);
  clearImage(a[3]);
  drawText(a[3],targw,p->xc,p->yc,1,255);
  setMovie(stim_m,3,a[3],p->duration);
  clearImage(a[5]);
  drawText(a[5],alt1w,p->xc-p->ahoriz,p->yc-p->avert,1,255);
  drawText(a[5],alt2w,p->xc+p->ahoriz,p->yc-p->avert,1,255);
  setMovie(stim_m,5,a[5],1);
  *data=runMovie(stim_m,UNTIL_RESPONSE,1);
  switch ((*data)->x[0].resp & 0x00ff){
  case 'z': *resp=0;break;
  case 'Z': *resp=0;break;
  case '/': *resp=1;break;
  case '@': *resp=2;break;
  default: *resp=3;break;}
  if (*resp==2) {cleanup();
  printf ("stopped while running by participant\n");exit(1);}
  if ((stim_m->x[5].ts)==0) *RT=-1;
  else *RT=((*data)->x[0].rt-stim_m->x[5].ts)*rtSlope;
  if (*RT<p->tooFast)
    {
      audio(ERROR);
      runMovieTimedText("TOO FAST! WAIT TO CONTINUE",320,240,1,255,360);
    }
  else
    {
      if (p->ans==*resp) audio(CORRECT);
      else audio(ERROR);
    }
  if (*resp==3)
    {
      runMovieText("INVALID KEY <reposition hands, hit spacebar to continue>",320,240,1,255);
    }
}


void runNameTrial(movie *stim_m, image **a, myParameters *p, response **data, char ***word)
{
  char targw[TopChar],alt1w[TopChar],alt2w[TopChar];
  char prime1w[TopChar],prime2w[TopChar];
    
  pad(targw,word[p->item][p->targ],"$");
  pad(prime1w,word[p->item][p->prime1],"$");
  pad(prime2w,word[p->item][p->prime2],"$");
  clearImage(a[2]);
  drawText(a[2],prime1w,p->xc,p->yc-p->vert,1,255);
  drawText(a[2],prime2w,p->xc,p->yc+p->vert,1,255);
  setMovie(stim_m,2,a[2],p->primetime);
  clearImage(a[3]);
  drawText(a[3],targw,p->xc,p->yc,1,255);
  setMovie(stim_m,3,a[3],p->duration);
  *data=runMovie(stim_m,UNTIL_RESPONSE,1);
  if (((*data)->x[0].resp & 0x00ff) == '@') 
    {
      cleanup();
      printf ("stopped while running by participant\n");
      exit(1);
    }
}

char *** malloc_word(int numI)
{
  int i,j;
  char *** word;
  word = (char ***)malloc(numI*sizeof(char **));
  for (i=0;i<numI;i++)
    {
      word[i]= (char **)malloc(6*sizeof(char *));
      for (j=0;j<6;j++)
	(word[i])[j]=(char *)malloc(TopChar*sizeof(char));
    }
    return word;
}

void load_word(int numI,char ***word, char *fname)
{
  FILE *infile;
  int i,it;

  infile=fopen(fname,"rt");

  for (it=0;it<numI;it++)
    for (i=0;i<6;i++)
      {
      fscanf(infile,"%s",word[it][i]);
      /*printf("%s\n",word[it][i]);*/
      }
  fclose(infile);  
}


void pad(char *dest, char *src, char *spacer)
{
  int l,frontpad,backpad,i;

  l=strlen(src);
  frontpad=5-(l/2);
  backpad=10-l-frontpad;
  strcpy(dest,"");
  for (i=0;i<frontpad;i++)
    strcat(dest,spacer);
  strcat(dest,src);
  for (i=0;i<backpad;i++)
  strcat(dest,spacer);
}



/*

      switch (data->x[0].resp & 0x00ff){
      case 'z': resp=0;break;
      case 'Z': resp=0;break;
      case '/': resp=1;break;
      case '@': resp=2;break;
      default: resp=3;break;}
      if (resp==2) {fclose(outfile);cleanup();
      printf ("stopped while running by participant\n");exit(1);}
      if ((stim_m->x[5].ts)==0) RT=-1;
      else RT=(data->x[0].rt-stim_m->x[5].ts)*rtSlope;
      if (RT<TOOFAST)  
	{
	  audio(ERROR);
	  runMovieTimedText("TOO FAST! WAIT TO CONTINUE",320,240,1,255,360);
	}
      else
	{
	  if ((target+alt)%2==resp) acc=1; else acc=0;
	  if (acc==1) 
	    {
	      audio(CORRECT); 
	      calcorrect[duration]++;
	    }
	 else audio(ERROR);
	}
      if (resp==3)
	{
	  runMovieText("INVALID KEY <reposition hands, hit spacebar to continue>",
	  320,240,1,255);
	}
      sprintf(outline,"sub%03d blk%02d trl%03d  item%03d prime%d targ%d alt%d dur%d %d %+f %d %d",
	log->subjnum,blk,i,oi[i],2,target,alt,durval[duration],resp,RT,acc,errorTotal); 
      fprintf(outfile,"%s\n",outline);
}


*/
