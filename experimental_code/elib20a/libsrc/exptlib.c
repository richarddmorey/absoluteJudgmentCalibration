#include "EXPTLIB.H"
#include "movieobj.h"

extern int errno;


void putsquare3(int size, int xc, int yc, int color, unsigned char far *b)
{
  putsquare(size,xc,yc,color,b);
  putsquare(size+1,xc,yc,color,b);
  putsquare(size-1,xc,yc,color,b);
}


void puttriangle3(double size, double xc, double yc, int color, unsigned char far *b)
{
  puttriangle(size,xc,yc,color,b);
  puttriangle(size-1,xc,yc,color,b);
  puttriangle(size+1,xc,yc,color,b);
}

void putcircle5(double rad, int xc, int yc, int color, unsigned char far *b)
{
  putcircle(rad,xc,yc,color,b);
  putcircle(rad+.35,xc,yc,color,b);
  putcircle(rad-.35,xc,yc,color,b);
  putcircle(rad+.7,xc,yc,color,b);
  putcircle(rad-.7,xc,yc,color,b);
}


void puttriangle(double size, double xc, double yc, int color, unsigned char far *b)
{
  int x[3], y[3], i;
  x[0]=xc-size;
  y[0]=yc+(1-M_SQRT1_2)*M_PI*size;
  x[1]=xc+size;
  y[1]=y[0];
  x[2]=xc;
  y[2]=yc-M_PI*size*M_SQRT1_2*.5;
  for (i=0;i<3;i++)
    putslopeline(x[i],y[i],x[(i+1)%3],y[(i+1)%3],color,b);
}

void putslopeline(int x1, int y1, int x2, int y2, int color, unsigned char far *b)
{

  double slope;
  int x,y;

  if (x2!=x1)
    {
      slope = (double)(y2-y1)/(double)(x2-x1);
      if (fabs(slope)>1)
	{
	  if (y2-y1>0)
	    for (y=y1;y<=y2;y++)
	      {
		x=x1+(y-y1)/slope;
		b[o(x,y)]=color;
	      }
	  else
	    for (y=y2;y<=y1;y++)
	      {
		x=x2+(y-y2)/slope;
		b[o((int)(x),(int)(y))]=color;
	      }
	}
      else
	{
	  if (x2-x1>0)
	    for (x=x1;x<=x2;x++)
	      {
		y=y1+(x-x1)*slope;
		b[o(x,y)]=color;
	      }
	  else
	    for (x=x2;x<=x1;x++)
	      {
		y=y2+(x-x2)*slope;
		b[o((int)(x),(int)(y))]=color;
	      }
	}
    }
}


void putsquare(int size, int xc, int yc, int color, unsigned char far *b)
{
  int i;
    
    for (i=0;i<size;i++)
    {
	b[(int)(o(((xc-size/2)+i),(yc-size/2)))]=color;
	b[(int)(o(((xc-size/2)+i),(yc+size/2)))]=color;
	b[(int)(o((xc-size/2),((yc-size/2)+i)))]=color;
	b[(int)(o((xc+size/2),((yc-size/2)+i)))]=color;
    }
}

void putrectangle(int hsize, int vsize, int xc, int yc, int color, unsigned char far *b)
{
  int i;
    
    for (i=0;i<hsize;i++)
      {
	b[(int)(o(((xc-hsize/2)+i),(yc-vsize/2)))]=color;
	b[(int)(o(((xc-hsize/2)+i),(yc+vsize/2)))]=color;
      }
    for (i=0;i<vsize;i++)
      {
 	b[(int)(o((xc-hsize/2),((yc-vsize/2)+i)))]=color;
	b[(int)(o((xc+hsize/2),((yc-vsize/2)+i)))]=color;
      }
}




void putcircle(double rad, int xc, int yc, int color, unsigned char far *b)
{
  int x,y,t;
  for (t=0;t<10*rad;t++)
    {
      x=xc+rad*cos((t*M_PI)/(5*rad));
      y=yc+rad*sin((t*M_PI)/(5*rad));
      b[o(x,y)]=color;
    }
}


void writeppm(int x1, int y1, unsigned char far *im, unsigned char far *ct)
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

void putrandombox(int num1, int num2, int size, int xc, int yc, int *val, unsigned char far *b, long *seed)
{
  int i;
  int *temp;
  int x,y;

  temp=(int *)malloc(size*size*sizeof(int));
  for (i=0;i<num1;i++) temp[i]=val[0];
  for (i=num1;i<num1+num2;i++) temp[i]=val[1];
  for (i=num1+num2;i<size*size;i++) temp[i]=val[2];
  if (*seed!=0) distribute(temp,(size*size),seed);
  for (i=0;i<size*size;i++)
    {
      x=(i%size)+(xc-size/2);
      y=(i/size)+(yc-size/2);
      b[o(x,y)]=temp[i];
    }
  free(temp);
}

void putcheckerbox(int size, int res, int xc, int yc, int *val, unsigned char far *b)
{
  int i;
  int *temp;
  int x,y;

  temp=(int *)malloc(size*size*sizeof(int));
  for (x=0;x<size;x++)
    for (y=0;y<size;y++)
      temp[x*size+y]=val[((x/res)+(y/res))%2];
  for (i=0;i<size*size;i++)
    {
      x=(i%size)+(xc-size/2);
      y=(i/size)+(yc-size/2);
      b[o(x,y)]=temp[i];
    }
  free(temp);
}
  

void putcross(int size, int xc, int yc, int color, unsigned char far *b)
{
  int x,y;

  for (x=xc-size/2;x<=xc+size/2;x++)
    b[o(x,yc)]=color;
  for (y=yc-size/2;y<=yc+size/2;y++)
    b[o(xc,y)]=color;
}

void putvline(int size, int x0, int y0, int color, unsigned char far *b)
{
  int y;

  for (y=y0;y<y0+size;y++)
    b[o(x0,y)]=color;
}

void puthline(int size, int x0, int y0, int color, unsigned char far *b)
{
  int x;

  for (x=x0;x<x0+size;x++)
    b[o(x,y0)]=color;
}


int assign(int *trial,int *num, int numofcond, long *seed)
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


void distribute(int q[], int size, long *seed)
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

int randint (int min, int max, long *seed)
{
 return((int)((min)+(max-min+1)*ran2(seed)));
}


double  gaussdev(long *seed)
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


float ran2(long *idum)
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
    long k;
    static long idum2 = 123456789;
    static long iy = 0;
    static long iv[NTAB];
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




int gen_init(int status, logtype *log)
{
    FILE  *testfile;
    char outfn[15];
    int returnval=0;

  log->seed=-(time(NULL)%10000);
  if (log->seed>0) log->seed=-1*log->seed;
  printf("EXPERIMENTER --- READ THE FOLLOWING CAREFULLY PLEASE\n");
  switch (status)
    {
    case UNLOGGED:
      printf("practice run, data to test.raw, no  log\n");
      sprintf(log->outfn,"test.raw");
      log->subjnum=0;
      getchar();
      break;
    case STAND_ALONE:     /* only working mode*/
      printf("EXPERIMENTER, BE SURE TO LOG PARTICIPANT MANUALLY\n");
      printf("Please enter output filename\n");
      gets(log->outfn);
      testfile=fopen(log->outfn,"rt");
      if (errno!=ENOFILE) {printf("output file may exist\n");exit(1);}
      printf("Please enter participant number\n");
      fscanf(stdin,"%d",&(log->subjnum));
      getchar();
      printf("\n file name is %s and subject number is %d\n",log->outfn,log->subjnum);
      printf("seed is %ld\n",log->seed);
      printf("hit any key to continue, control-c to escape\n");
      getchar();
      break;
  }
  return(returnval);
}





void thankyou()
{
 movie *thank_m;
 image *thank_i;
 int code=0;
 response *data; int resp; long RT;


 thank_i=newImage();
 downloadImage(thank_i);
 drawText("-------------The End------------",0,0,0,255);
 drawText("Thank You For Your Participation",0,25,0,255);
 uploadImage(thank_i);
 thank_m=initMovie(1);
 setMovie(thank_m,0,thank_i,1);
 do
   {
     data=runMovie(thank_m, UNTIL_RESPONSE,2);
     if ((data->x[0].resp=='x') && (data->x[1].resp='@')) code=1;
   } while (code==0);
}

void audio(int feedback, movie *wrong_m)
{
  switch (feedback)
    {
    case CORRECT:
      sound(500);
      delay(5000);
      nosound();
      sound(1000);
      delay(5000);
      nosound();
      break;
    case ERROR:
      sound(100);
      delay(10000);
      nosound();
      break;
    case INVALID:
      sound(100);
      runMovie(wrong_m,FULL_SHOW,0);
      delay(20000);
      nosound();
      break;
    }
}

