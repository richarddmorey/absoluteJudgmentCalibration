#ifndef _RESPONSE_H
#define _RESPONSE_H

/*
 * Response interface.
 */


typedef struct {
  int numResponses;
  int responsesMade;
  int useParPort;
  struct response_elem {
    long long unsigned int rt;
    int resp;
  } *x;
} response;

response *initResponse(int numResponses);

void freeResponse(response *r);

int responseReady(response *r);

void checkResponse(response *r);

#endif /* _RESPONSE_H */

