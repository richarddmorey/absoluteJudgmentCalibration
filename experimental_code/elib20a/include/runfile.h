
/*
 * Cheap-ass error handler.
 * Switches to text mode, prints what it was passed, and quits.
 */

void error(const char *fmt, ...);

/*
 * Desc: Image interface.
 */

#include <mem.h>

#define NUM_IMAGES 128

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define IMAGE_DEPTH  1
#define IMAGE_SIZE   307200;

typedef BITMAP*  image;

/* Setup/destroy image data structures. */
void goImages(void);
void killImages(void);

/* Creates a new, blank, image. (Implicitly clears the picBuf). */
image *newImage(void);

/* The picture bufffer is a playground where the image can be directly
   manipulated. Use down/up to transfer playground data from/to image
   structures. The image should already have been created by new or load. */
extern BITMAP *picBuf;
void  downloadImage(image *im);
void  uploadImage(image *im);

#define clearPicBuf() clear_bitmap(picBuf);
#define fillPicBuf(x) clear_to_color(picBuf,(x))

/* To draw text in the picture buffer  */
void  drawText(const char *text, int x, int y, int font, int color);

/* File operations, load and save images. */
#define PPM  1
image *loadImage(const char *fname, int type);
void  saveImage(image *im, const char *fname, int type);


/*
 * Response interface.
 */

typedef struct {
  int numResponses;
  int responsesMade;
  struct response_elem {
    long long unsigned int rt;
    int resp;
  } *x;
} response;

response *initResponse(int numResponses);

void freeResponse(response *r);

int responseReady(response *r);

void checkResponse(response *r);

/* 
 * Direct access to graphics.
 */

void drawPicture(BITMAP *b, response *r);

void setPicSettings(BITMAP *picbuf, int w, int h);

/* the mode classes */
#define TEXT_MODE     0
#define GRAPHICS_MODE 1

void clearScreen  (int color);

void setDisplayParams(int width, int height, int refr);

void setVideoMode (int mode);

void FlipPages(int wait);

int  Retrace(void);

#define PIC_POS_X 0
#define PIC_POS_Y 0


extern unsigned int Palette[256];
void setPalette(void);
void writePalette(const char *fname);
void readPalette(const char *fname);
int  paletteSize();

#define PALETTE_FULL -1
int  addColor(unsigned int p);

#define CLEAR     0
#define GRAYSCALE 1
#define EMPTY     2
void makePalette(int paltype);

#define NOT_FOUND -1
int findColor(unsigned int p);

#define MAX_COLORS 256
#define MAX_COLOR_INTENSITY 256

/*
 * Movie Library Interface
 *
 */

/* The movie data structure. */
typedef struct {
  int numImages;
  struct movie_image {
    long long int ts; /* time stamp */
    image *im;
    int numFrames;
  } *x;
} movie;

/*
 * Image/Movie Functions
 */

#define FULL_SHOW      1
#define UNTIL_RESPONSE 2
movie *initMovie(int numImages);
void  setMovie(movie *m, int pos, image *im, int numFrames);
response *runMovie(movie *m, int showType, int numResponses);

/* 
 * Administration
 */

void SetupMoviePackage(const char *fontlistfname);
void CleanupMoviePackage(void);

/*
 * File: input.h
 * Interface to the Cogsys input module. 
 *
 */

void FlushKeyBuf(void);       /* formerly keyflush() */
int  KeyWaiting(void);        /* formerly keyscan()  */
int  GetKey(void);            /* formerly getkey()   */
int  KeyPress(void);          /* formerly keypress() */
int  GetStick(void);          /* formerly getstick() */
char *ReadString(void);

#define NOTHING_PRESSED    0xF0
#define JOY1BUT1   0xE0
#define JOY1BUT2   0xD0
#define JOY2BUT1   0xB0
#define JOY2BUT2   0x70

/* this strange looking constant has heavy use
   in legacy commands. */
#define JOY1BUT1_AND_JOY2BUT1 0xA0

#define CHECK_JOY1BUT1     0x10
#define CHECK_JOY1BUT2     0x20
#define CHECK_JOY2BUT1     0x40
#define CHECK_JOY2BUT2     0x80
