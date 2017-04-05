/*
 * Response implementation.
 */

#include <allegro.h>
#include "response.h"
#include "input.h"

#include <stdlib.h>
#include <stdio.h>

#include "tsc_time.h"
#include "parport.h"

extern long long int DIOTime;
char DIOKeys[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
char btParout [4] ={ PP_RESPONSE1, PP_RESPONSE2, PP_RESPONSE3, 0};

response *initResponse(int numResponses)
{
  response *r;
  r = (response *) malloc(sizeof(response));
  r->numResponses = numResponses;
  r->responsesMade = 0;
  r->useParPort = 0;
  r->x = (struct response_elem *) calloc(numResponses, 
					 sizeof(struct response_elem));
  return r;
}

void freeResponse(response *r)
{
  free(r->x);
  free(r);
}

void checkResponseO(response *r)
{
  if (r->responsesMade < r->numResponses && keypressed())
    {
      r->x[r->responsesMade].resp = readkey();
      r->x[r->responsesMade].rt   = read_tsc();
      r->responsesMade++;
    }
}

void checkResponse(response *r)
{
  int rc;
  int k, p, n;
  if (r->responsesMade < r->numResponses) {
    if ((rc = DIO24_pressed()) != 0) {
/*   printf(" %x\n", rc); */
      if (rc & 0x001f) {
        if (rc & 0x0007) {
          if (rc & 0x0003) {
            if (rc & 0x0001) {
               k = 0; p = 1;
            }else {
               k = 1; p = 2;
            }
          }else {
            k = 2; p = 4;
          } 
        }else if(rc & 0x0008){
          k = 3; p = 8;
        } else {
          k = 4; p = 16;
        }
      }else{ 
        if (rc & 0x0700) {
          if (rc & 0x0300) {
            if (rc & 0x0100) {
              k = 5; p = 256;
            } else {
              k = 6; p = 512;
            }
          }else {
           k = 7; p = 1024;
          }
        }else if (rc & 0x0800) {
          k = 8; p = 2048;
        }else {
          k = 9; p = 4096;
        }
      } 
      if (rc != p) rc |= 0x0000c000; else rc |= 0x00008000; 
      r->x[r->responsesMade].resp = DIOKeys[k] + (rc << 16);
      r->x[r->responsesMade].rt   = DIOTime;
      n = r->responsesMade;
      if(r->useParPort){
        if(n<3)outParPort(btParout[n]);
      }
      r->responsesMade=n+1;
      refreshParPort();
      return;
    }
    if (keypressed())      {
        r->x[r->responsesMade].resp = readkey();
        r->x[r->responsesMade].rt   = read_tsc();
	n = r->responsesMade;
	if(r->useParPort){
          if(n<3)outParPort(btParout[n]);
        }
	r->responsesMade=n+1;
    }
  }
  refreshParPort();
}

	      
int responseReady(response *r)
{
  checkResponse(r);
  return (r->numResponses == r->responsesMade);
}
