#include<dpmi.h>
#include<sys/farptr.h>
#include<pc.h>
#include<go32.h>
#include "tsc_time.h"
#include "parport.h"


static unsigned short ParPort = 0; 

static int numActive = 0;

static char valQuee[8];
static long long unsigned int timeQuee[8];
static int headQuee = 0;

static char portMirror = 0;
static char bg = 0;
static int pulseDuration = 0;

static newFlg;

/* Clears all data bits of previously selected port, inits internal data */
/* Sets required value of pulse length in machine cycles                 */
/* Input parameter - required pulse length in machine cycles             */
/* Return value - port base address                                      */
int initParPort(int Duration)
{
  if(ParPort == 0) return 0;

  bg = 0;

  numActive = 0;
  headQuee = 0;

  newFlg = 0;

  pulseDuration = Duration;

  portMirror = 1; /* to force output of zeros in refreshParPort */

  refreshParPort();
  
  return(ParPort);
}

/* Schedules output of a pulse on par.port */
void outParPort(char val)
{
  valQuee[headQuee] = val;
  newFlg = 1; /* flag tells refreshParPort to finish processing */
  refreshParPort();  
  return;
}

/* These two functions can be used to change level of some lines   */
void setParPortbg(char val)
{
  bg |= val;
  refreshParPort();
}

void clrParPortbg(char val)
{ 
  bg &= val;
  refreshParPort();
}

/* Heart of the module - the only place where actual output is performed */
/* All other ParPort programs just schedule required actions, this one   */
/* performs them;                                                        */
/* Should be called repeatedly;                                          */
/* Return value - number of pending pulses.                              */
int refreshParPort()
{
  char val;
  int pos, cnt, dt;
  long long unsigned int time;

  if (ParPort == 0) return(0); /* Nothing to do */

  time = read_tsc(); /* Current timestamp im machine cycles */
  
  val = (newFlg?(bg | valQuee[headQuee]):bg);
	
  
  pos = headQuee;
  for(cnt = numActive;cnt >0; cnt--){
     pos =(pos -1)&0x03;
     dt = time - timeQuee[pos];
     if (dt < pulseDuration){
       /* still active */
       val |= valQuee[pos];
     }
     else{
       numActive -= cnt;
       break;
     }
  }
  if (portMirror != val){
    outportb(ParPort, val);
    portMirror = val;
  }
  
  if (newFlg){
    timeQuee[headQuee] = time;
    headQuee = (headQuee+1)&0x03;
    numActive++;
    newFlg = 0;
  }
  
  return (numActive);
}

/* Waits until all pending pulses finished */
void finParPort()
{
   while(refreshParPort())
	   ;
}

/* Sets parallel port base address from BIOS area                   */
/* Input - port number starting from 1 (Lpt1 - 1 Lpt2 - 2, Lpt3 - 3 */
/* On some computers BIOS area might be incorrect, especially       */
/* if requested parallel port does not exist.                       */ 
int selectParport(int No)
{
  No--;
  if((No < 0)||(No >3)){
    ParPort = 0;
    return (-1);
  }
  ParPort = _farpeekw(_dos_ds, 0x408 +2*No); /* Port base adress */
  if (ParPort == 0xffff) ParPort = 0; /* definetly incorrect */
  if (ParPort == 0) return (-1);      
  return(ParPort);
}

/* Can be used to manually set ParPort base address, use with care!   */
/* Makes no sanity checks and can lead to disaster if value is incorrect */
void setParPortAdr(int Adr)
{
	ParPort = Adr;
}

/* Just for completeness - returns currently set ParPort base address */
/* Not used                                                           */
int parPortAdr()
{
	return ParPort;
}


