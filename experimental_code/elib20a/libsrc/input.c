/*
 * File: input.c
 * Cogsys input module; keyboard and game port.
 */

#include <allegro.h>

#include "input.h"
#include "tsc_time.h"

extern short DIO24r1, DIO24r2;
extern int   UseDIO24;

int DIOThr = 30000;
static int   DIOstat;
static int DIOm1 = 0, DIOm2=0x00001F1F;
long long int DIOTime;
static long long int LDIOTime;
int DIO24mask = 0x00001F1F; /* Default - all keys */

/*
 * Function FlushKeyBuf
 * Flushes the keyboard buffer.
 */

void FlushKeyBuf()
{
  clear_keybuf();
}

void FlushDIO24()
{
  if (DIO24r2){
     DIOstat = (~inportl(DIO24r2))&DIO24mask;
     DIOTime = LDIOTime = read_tsc();
     DIOm1 = DIOm2 = 0;
  }
}


/*
 * Function KeyWaiting()
 * Return true if a key is waiting in buffer.
 */
int KeyWaiting()
{
  return keypressed();
}

/*
 * Function GetKey
 * Return first character in keyboard buffer.
 */
int GetKey()
{
  return readkey();
}

/*
 * Function KeyPress()
 *
 * This function continuously scans the keyboard until a key is
 * pressed.  It then returns the ASCII code of that key.
 */
int KeyPress()
{
  /* Flush buffer */
  clear_keybuf();
  /* Wait until keypress. */
  /* Return key pressed. */
  return readkey();
}


int DIO24_pressed()
{
  unsigned int rc, val;
  if(DIO24r2 == 0) return 0;
  val = (~inportl(DIO24r2)) & DIO24mask;
  DIOTime = read_tsc();
  rc = (~DIOstat) & val;  /* 0 -> 1*/
  DIOstat |= val;
  DIOm1 |= val;
  DIOm2 |= val;
  if (DIOTime - LDIOTime > DIOThr){
     DIOstat &= DIOm2; /* reset keys that were not pressed long enough */
     DIOm2 = DIOm1;
     DIOm1 = 0;
     LDIOTime = DIOTime; /* */
  }
  return (rc);
}

/*
 * Function GetStick
 * Uses BIOS to read joystick port. Returns value of joystick trigger.
 */
/* Fixme */
#if 0
int GetStick()
{
  if(poll_joystick()) return 0;  
  return joy.button[0].b;
}
#endif
  
