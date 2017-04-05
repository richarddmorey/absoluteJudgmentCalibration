#ifndef _GRAPH_H
#define _GRAPH_H

extern int vpage, apage;


int InitGraphics(int x, int y, int bits);

int initSNAP();

int initGraph(int XX, int YY, int RefrR, int dpth);

void exitGraph();

void exitSNAP();

void bitBltMem(
		void *src,
		int sPitch,
		int sLeft,
		int sTop,
		int width,
		int hight,
		int dLeft,
		int dTop);
int Retrace();

void setActiveP(
		int page);

void setVisualP(
		int page,
		int wait);

void getP(
		void *pal,
		int num,
		int index);

void setP(
		void * pal,
		int num,
		int index,
		int waitVRT);

#endif /* _GRAPH_H */

