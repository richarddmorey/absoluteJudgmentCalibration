#ifndef _ERROR_H
#define _ERROR_H

/*
 * Cheap-ass error handler.
 * Switches to text mode, prints what it was passed, and quits.
 */

void error(const char *fmt, ...);

#endif /* _ERROR_H */
