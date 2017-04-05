#define M_PI 3.14159265358979323846
#define M_SQRT1_2 0.70710678118654752440
#define CORRECT 0
#define ERROR 1
#define DEBUG 0
#define RUN 1
#define SWAP(a,b) {swap = (a) ; (a) = (b) ; (b) = swap ;}


typedef struct
{
  char outfn[15];
  char backfn[20];
  int subjnum;
  int seed;
  int gender;
  int ethnic;
  int year;
  int month;
} logtype;


void runMovieText(char *message,int x, int y, int font, int color);
void runMovieTimedText(char *msg, int x, int y, int font, int color, int frames);
void writeppm(int x1, int y1, unsigned char *im, unsigned char *ct);
int assign(int *trial,int *num, int numofcond, int *seed);
/* assign assumes you know the number of each type of trial.
 *trial is the pointer to an array with each trials type.
 *num is an array of length numofcond and contains the number
  of trial for each condition type.  The resulting order of
  trial types is randomized*/
void distribute(int q[], int size, int *seed);
/* Randomize an array of integers.  q is an array and size is its size
note- this routine needs randint*/
int randint(int min, int max, int *seed);
double gaussdev(int *seed);
float ran2(int *idum);
/*Produces uniform random deivaites between 0 and 1
From Numerical Recipes.  Initialize with a big negative number.
ex.
seed=-time(NULL)%10000;
x=ran2(&seed);
y=ran2(&seed);
x and y are uniform deviates between 0 and 1 (exclusive)*/

void thankyou();
/*void audio(int feedback, movie *wrong_m);*/
void initLog(int status, logtype *log);
void audio(int feedback);


