#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "tsc_time.h"


#define STEP 0
#define DIM 50
#define NUM 5
#define DEFPERIOD 60

int tsc_NumPoints = 0;
long long unsigned int tsc_rt[DIM], tsc_cnt[DIM];
long double tsc_Slope, tsc_T0;
int tsc_flg = 0;

int main(int argc, char * argv[])
{
	struct timeval first, current;	
	struct timezone tz;
	long long unsigned int timestamp, tme;
	long double t1[100], t2[100], tc1, tc2, d;   

	int i, cod, stp=0;
	unsigned int slp;
	tsc_NumPoints = 0;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
        int period = DEFPERIOD;

        if (argc >2){
           printf("Usage:\n\tclcrate [<period in sec>]\n");
	   exit;
	} 
        else if (argc == 2){
	   sscanf(argv[1], "%i", &period);
	   if (period <=0) period = DEFPERIOD; 
	}
	cod = gettimeofday(&first, &tz);
	printf (" %i %i %li \n", cod, first.tv_sec, first.tv_usec);

	tsc_FirstEstim();
	sleep(period);
	tsc_Estim();

	printf("Slope m = %25.15Le, b = %25.15Le \n", tsc_Slope, tsc_T0);

	for (i = 0; i < tsc_NumPoints; i++){
		d = tsc_rt[i]/1000000.0 - tsc_Slope*tsc_cnt[i] - tsc_T0;
		printf ("%L25.15g\n", d);
	}
#if (0)
	for (i=0;i<10;i++){
		for(;;){
			gettimeofday(&current, &tz);
			if(current.tv_usec != first.tv_usec){
				timestamp = read_tsc();
				first.tv_sec = current.tv_sec;
				first.tv_usec = current.tv_usec;
				if(stp == 32){
					stp = 0;
				
					tc1 = current.tv_sec + current.tv_usec/1000000.0;

					tc2 = (timestamp*tsc_Slope +tsc_T0);
					printf("Timer %20.4Lf RT %20.4Lf Dlt %15.4Lf \n", tc1, tc2, tc2 - tc1);
					break;
				} else stp++;

			}
		}
	}
#endif

	
	return 0;
}

void GetTimePoint(int num)
{
	int stp, i;
	long long unsigned tme, timestamp;
	struct timezone tz;
	struct timeval first, current;

	stp = -1; /* To skip the first */
	for (i = 0; i< num; i++){
		if(tsc_NumPoints >= DIM) error("Time buffer overflow\n");
		for(;;){
			gettimeofday(&current, &tz);
			if(current.tv_usec != first.tv_usec){
				timestamp = read_tsc();
				first.tv_sec = current.tv_sec;
				first.tv_usec = current.tv_usec;
				if(stp == STEP){
					stp = 0;
					
                                	tme = current.tv_sec;
					tsc_rt[tsc_NumPoints] = 1000000*tme + current.tv_usec;
					tsc_cnt[tsc_NumPoints] = timestamp;
					tsc_NumPoints++;
					break;
				} else stp++;

			}
		}
	}
	tsc_flg++;
}

/* Linear regression - Time = tsc_cnt*tsc_Slope + tsc_T0 */
void CalcTimeParams(void)
{
	long double s1, s2, s12, s11, s22, tc1, tc2, b1, d;
	long long unsigned int rt0, ts0;
	int i;

	rt0 = tsc_rt[0];
	ts0 = tsc_cnt[0];
	s1 = s2 = s12 = s11 = s22 = 0.0;

	if (tsc_NumPoints < 2) error("Can not calculate time conversion parameters - not enough data\n"); 
	for (i = 0; i < tsc_NumPoints; i++){
		tc1 = tsc_rt[i] - rt0;
		tc2 = tsc_cnt[i] - ts0;
		s1 += tc1;
		s2 += tc2;
		s11 += tc1*tc1;
		s12 += tc1*tc2;
		s22 += tc2*tc2;
	}

	tsc_Slope = (tsc_NumPoints*s12 -s1*s2)/(tsc_NumPoints*s22-s2*s2); /* slope */

	b1 = (s1 - tsc_Slope*s2)/tsc_NumPoints;

	tsc_T0  = (rt0 - (tsc_Slope*ts0 - b1))/1000000.0;
	tsc_Slope = tsc_Slope/1000000.0;

}

void tsc_FirstEstim()
{
	GetTimePoint(NUM);
	sleep(1);
	GetTimePoint(NUM);
	CalcTimeParams();
	tsc_flg = 1;
}	
	
void tsc_Estim()
{
	if (tsc_flg == 0) error("Error in tsc_Estim\n");
	GetTimePoint(NUM+NUM);
	CalcTimeParams();
	tsc_flg = 2;
}	
