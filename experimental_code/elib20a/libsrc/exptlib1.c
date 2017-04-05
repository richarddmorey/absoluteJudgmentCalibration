#include <allegro.h>
#include <alfont.h>
#include <errno.h>
#include <stdio.h>
#include <dos.h>
#include "response.h"
#include "exptlib.h"
#include "movieobj.h" 

extern int errno;
extern int errorTerm;
extern movie *std_m;
extern image *std_i;


void runMovieText(char *message,int x, int y, int font, int color)
{
  clearImage(std_i);
  drawText(std_i,message,x,y,font,color);
  setMovie(std_m,0,std_i,1);
  runMovie(std_m,UNTIL_RESPONSE,1);
}

void runMovieTimedText(char *message,int x, int y, int font, int color, int frames)
{
  clearImage(std_i);
  drawText(std_i,message,x,y,font,color);
  setMovie(std_m,0,std_i,frames);
  runMovie(std_m,FULL_SHOW,0);
}


#if 0
void writeppm(int x1, int y1, unsigned char *im, unsigned char *ct)
{
  int i,j,k;
  printf("P3\n");
  printf("#  CREATOR:  Jeff Rouder 8/99\n");
  printf("%d %d\n",x1,y1);
  printf("255\n");
  for (j=0;j<y1;j++)
    {
      for (i=0;i<x1;i++)
	printf("%3d %3d %3d  ",
	       ct[(im[o(i,j)]*3)],ct[(im[o(i,j)]*3)+1],ct[(im[o(i,j)]*3)+2]);
      printf("\n");
    }
}
#endif

int assign(int *trial, int *num, int numofcond, int *seed)
{
  int c, i,j;


  c=0;
  for (i=0;i<numofcond;i++)
    for (j=0;j<num[i];j++)
      {
	trial[c]=i;
	c++;
      }
  if (*seed != 0) distribute(trial,c,seed);
  return(c);
}


void distribute(int q[], int size, int *seed)
{

  int c,newplace,temp;

  for (c=0;c<size;c++)
    {
      newplace=randint(c,size-1,seed);
      temp=q[newplace];
      q[newplace]=q[c];
      q[c]=temp;
    }
}

int randint (int min, int max, int *seed)
{
 return((int)((min)+(max-min+1)*ran2(seed)));
}


double  gaussdev(int *seed)
{
  double r1,r2,y1,y2,y3;
  static double store;
  static int flag=0;

  flag=(flag+1)%2;
  if (flag)
    {
      r1 = ran2(seed);
      r2 = ran2(seed);
      y1 = sqrt(-2*log(r1));
      y2 = cos(2*M_PI*r2);
      y3 = sin(2*M_PI*r2);
      store=y3*y1;
      return(y1*y2);
    }
  else 
    return(store);
}


float ran2(int *idum)
{
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
    int j;
    int k;
    static int idum2 = 123456789;
    static int iy = 0;
    static int iv[NTAB];
    float temp;

    if (*idum<=0)
      {
       if (-(*idum)<1) *idum=1;
       else *idum=-(*idum);
       idum2=(*idum);
       for (j=NTAB+7;j>=0;j--)
	{
	 k=(*idum)/IQ1;
	 *idum=IA1*(*idum-k*IQ1)-k*IR1;
	 if (*idum<0) *idum +=IM1;
	 if (j<NTAB) iv[j]=*idum;
	}
       iy=iv[0];
      }
    k=(*idum)/IQ1;
    *idum=IA1*(*idum-k*IQ1)-k*IR1;
    if (*idum<0) *idum += IM1;
    k = idum2/IQ2;
    idum2 = IA2*(idum2-k*IQ2)-k*IR2;
    if (idum2 < 0) idum2 += IM2;
    j=iy/NDIV;
    iy=iv[j]-idum2;
    iv[j]=*idum;
    if (iy<1) iy +=IMM1;
    if ((temp=AM*iy) > RNMX) return RNMX;
    else return temp;
}

void thankyou()
{
 movie *thank_m;
 image *thank_i;
 int code=0;
 response *data; int resp; int RT;


 thank_i=newImage();
 drawText(thank_i,"-------------The End------------",0,0,0,255);
 drawText(thank_i,"Thank You For Your Participation",0,25,0,255);
 thank_m=newMovie(1);
 setMovie(thank_m,0,thank_i,1);
 do
   {
     data=runMovie(thank_m, UNTIL_RESPONSE,2);
     if ((((data->x[0].resp)&0x00ff)=='x') && (((data->x[1].resp)&0x00ff=='@'))) code=1;
   } while (code==0);
}

void audio(int feedback)
{
  switch (feedback)
    {
    case CORRECT:
      sound(500);
      delay(100);
      nosound();
      sound(1000);
      delay(100);
      nosound();
      break;
    case ERROR:
      sound(100);
      delay(200);
      nosound();
      break;
    }
}

void initLog(int status, logtype *log)
{
    FILE  *testfile;

  log->seed=-(time(NULL)%10000);
  if (log->seed>0) log->seed=-1*log->seed;
  printf("EXPERIMENTER --- READ THE FOLLOWING CAREFULLY PLEASE\n");
  switch (status)
    {
    case DEBUG:
      errorTerm=1;
      printf("practice run, data to test.raw, no log\n");
      sprintf(log->outfn,"test.raw");
      log->subjnum=0;
      getchar();
      break;
    case RUN:     /* only working mode*/
      errorTerm=0;
      printf("EXPERIMENTER, BE SURE TO LOG PARTICIPANT MANUALLY\n");
      printf("Please enter output filename\n");
      gets(log->outfn);
      testfile=fopen(log->outfn,"r");
      if (testfile) error("output file may exist\n");
/*      else printf("Error code %d\n",errno);
      if (errno!=ENOENT) {printf("output file may exist\n");exit(1);} */
      sprintf(log->backfn,"C:\\BACK\\%s",log->outfn);
      printf("Please enter participant number\n");
      fscanf(stdin,"%d",&(log->subjnum));
      getchar();
      printf("Please enter participant gender [M=0/F=1]\n");
      fscanf(stdin,"%d",&(log->gender));
      getchar();
      printf("Please enter participant ethnicity\n");
      printf("WHITE=0, AF.AM.=1, HISP=2, HISPNW=3, NATAMER=4, ASIAN=5, NA=6, OTHER=7\n");
      fscanf(stdin,"%d",&(log->ethnic));
      getchar();
      printf("Age: rounded down to nearest year\n");
      fscanf(stdin,"%d",&(log->year));
      getchar();
      printf("Age: additional months\n");
      fscanf(stdin,"%d",&(log->month));
      getchar();
      printf("\nFile name is %s\n",log->outfn);
      printf("Backup file name is %s\n",log->backfn);
      printf("Subject number is %d\n",log->subjnum);
      printf("Gender is %d\n",log->gender);
      printf("Ethnicity is %d\n",log->ethnic);
      printf("Age is %d Years + %d Months\n",log->year,log->month);
      printf("Seed is %ld\n\n",log->seed);
      printf("hit any key to continue, control-c to escape\n");
      getchar();
      break;
  }
}
