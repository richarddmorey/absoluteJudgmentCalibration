
/*
 * Cheap-ass error handler.
 * 
 * Tries to call clean-up, switches to text mode, prints what it was
 * passed, and quits.  
 */

#include "error.h"
/* #include "movieobj.h" */
#include "response.h"
#include "video.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

void FatalErrorCleanup(void);

extern int SNAPon;

void error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  setVideoMode(TEXT_MODE);
  clearScreen(0);
  if (SNAPon) FatalErrorCleanup();  
  vprintf(fmt, ap);
  va_end (ap);
  exit(2);
}
