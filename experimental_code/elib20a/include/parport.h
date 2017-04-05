#ifndef _PARPORT_H
#define _PARPORT_H

/* definitions for bits in ParPort output byte */
#define PP_MOVIE_STARTED  (1)
#define PP_MOVIE_FINISHED (2)
#define PP_MOVIE_POINT1   (4)
#define PP_MOVIE_POINT2   (8)
#define PP_MOVIE_POINT3  (16)
#define PP_RESPONSE1     (32)
#define PP_RESPONSE2     (64)
#define PP_RESPONSE3    (128)


/* Sets parallel port base address from BIOS area                   */
/* Input - port number starting from 1 (Lpt1 - 1 Lpt2 - 2, Lpt3 - 3 */
/* On some computers BIOS area might be incorrect, especially       */
/* if requested parallel port does not exist.                       */
int selectParport(int No);

/* Can be used to manually set ParPort base address, use with care!   */
/* Makes no sanity checks and can lead to disaster if value is incorrect */
void setParPortAdr(int Adr);

/* Just for completeness - returns currently set ParPort base address */
/* Not used                                                           */
int parPortAdr();

/* Clears all data bits of previously selected port, inits internal data */
/* Sets required value of pulse length in machine cycles                 */
/* Input parameter - required pulse length in machine cycles             */
/* Return value - port base address                                      */
int initParPort(int Duration);

/* Schedules output of a pulse on par.port */
void outParPort(char val);

/* These two functions can be used to change level of some lines   */
void setParPortbg(char val);
void clrParPortbg(char val);

/* Heart of the module - the only place where actual output is performed */
/* All other ParPort programs just schedule required actions, this one   */
/* performs them;                                                        */
/* Should be called repeatedly;                                          */
/* Return value - number of pending pulses.                              */
int refreshParPort();

/* Waits until all pending pulses finished */
void finParPort();
	
#endif /* _PARPORT_H */
