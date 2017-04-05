#include <stdio.h>
#include <stdlib.h>
int calc(double v, double k1, double k2, double k3, int mx, int *res1, int *res2, int *res3)
{
      double vmn, lmx, dlt, ldlt, absdlt;

      int i1, i2, i3, n1, n2, n3, o1, o2, o3;

	
        o1=o2=o3=-1;
	n1 = (v/k1);
	n1++;
	if (n1 > mx) n1 = mx;
	lmx = 10000000.0;	
	for (i1 = n1; i1 >= 0; i1--){
	  n2 = (v - i1*k1)/k2;
	  n2++;
	  if (n2 >mx) n2 = mx;
	  for (i2 = n2; i2 >= 0; i2--){
	    n3 = (v - i1*k1 -i2*k2)/k3;
	    n3++;
	    if (n3 >mx) n3 = mx;
	    ldlt = 1000.0; /* infinity */
	    for(i3 =n3;i3 >=0; i3--){
	      dlt = v -( i1*k1+i2*k2+i3*k3);
	      absdlt = (dlt >= 0.0 ? dlt: -dlt);
	      if (absdlt < lmx){ 
	        lmx = absdlt;
		o1 = i1;
		o2 = i2;
		o3 = i3;
	      }
	      if (absdlt > ldlt) goto out3;
	      ldlt = absdlt;
            }
            out3:
	  }
          out2:
	}
        out1:
	*res1 = o1;
	*res2 = o2;
	*res3 = o3;
        return o1; 
}

int main (void)
{
   double k1, k2, k3, v;
   int mx = 255, r1, r2, r3;
   
   k1 = 16.0;
   k2 = k1/(4.0*1.4142);
   k3 = k2/(4.0*1.4142);
   for (v = 0.0; v < (mx+1)*16.0; v++){
      calc(v, k1, k2, k3, mx, &r1, &r2, &r3);
      printf ("v %lf  %lf %d %d %d\n", v, (double)(k1*r1 +k2*r2 +k3*r3), r1, r2, r3); 
   }
}

