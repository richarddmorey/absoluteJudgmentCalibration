
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
