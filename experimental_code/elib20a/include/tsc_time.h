#ifndef _TSC_TIME_H
#define _TSC_TIME_H

/* Access to real time counter on Pentium (+) processors */

static inline unsigned long long read_tsc(void)
{ 
    unsigned long high, low;
     __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));
     return ((unsigned long long)high << 32) | low; 
}

void GetTimePoint(int num);

void CalcTimeParams(void);

void tsc_FirstEstim();

void tsc_Estim();

void GetTimeParams(void);

#endif /* _TSC_TIME_H */
