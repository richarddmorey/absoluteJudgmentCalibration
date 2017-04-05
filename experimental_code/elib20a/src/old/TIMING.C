#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "tsc_time.h"



extern int tsc_NumPoints;
extern long long unsigned int *tsc_rt, *tsc_cnt;
extern long double tsc_Slope, tsc_T0;


int main()
{
        FILE *clkParms;
	struct timeval first, current;	
	struct timezone tz;
	long long unsigned int timestamp, tme;
	long double t1[100], t2[100], tc1, tc2, d;   

	int i, cod, stp;
	unsigned int slp;
	tsc_NumPoints = 0;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	cod = gettimeofday(&first, &tz);
	printf (" %i %i %li \n", cod, first.tv_sec, first.tv_usec);

	tsc_FirstEstim();
	sleep(180);
	tsc_Estim();
        
	printf("Slope m = %25.15Le, b = %25.15Le \n", tsc_Slope, tsc_T0);
        clkParms = fopen("c:/sysclk.dat","w");
        if (clkParms == 0) {
          printf("Error - can not create output file \n");
	  exit;
        }
        fprintf(clkParms, "CLK %25.15Le %25.15Le \n",tsc_Slope, tsc_T0);
        fclose(clkParms);

        stp = 0;
	for (i=0;i<10;i++){
		for(;;){
			gettimeofday(&current, &tz);
			if(current.tv_usec != first.tv_usec){
				timestamp = read_tsc();
				first.tv_sec = current.tv_sec;
				first.tv_usec = current.tv_usec;
				if(stp == 30){
					stp = 0;
				
					tc1 = current.tv_sec + current.tv_usec/1000000.0;

					tc2 = (timestamp*tsc_Slope +tsc_T0);
					printf("Timer %20.4Lf RT %20.4Lf Dlt %15.4Lf \n", tc1, tc2, tc2 - tc1);
					break;
				} else stp++;

			}
		}
	}


	
	return 0;
}

