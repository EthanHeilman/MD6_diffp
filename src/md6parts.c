#include <stdio.h>

const int t0=17, t1=18, t2=21, t3=31, t4=67, t5=89; // tap positions
#define n 89                                        // register size
#define w 64                                        // word size
#define c 16                                        // round size
#define md6gamma 5

#define min(a,b) ((a)<(b)? (a) : (b))
#define max(a,b) ((a)>(b)? (a) : (b))

int i2, i3, i4;

int ComputeAndGatesActivated(int D[], int i){

		int newaag = D[i];

		// if i>=(t4-t2): then there is enough room for t4,t3
		if (i>=(t4-t2)) {
			i3 = i-(t3-t2);
			i4 = i-(t4-t2);  
			if (D[i] > max(D[i3],D[i4]))         
				newaag += D[i]-(max(D[i3],D[i4])); 
		}


    // count new active AND gates that are within our
    // window, and which have their right input
    // on position i, and which weren't counted before
    if (i>=(t4-t3) && D[i]>D[i-(t4-t3)]) 
			newaag += D[i]-D[i-(t4-t3)];

		// all diff tap positions have enough room to exist all at once
    if (i>=(t4-t1)) { 
      i2 = i-(t2-t1);
      i3 = i-(t3-t1);
      i4 = i-(t4-t1);
      if (D[i] > max(D[i2],max(D[i3],D[i4])))      
        newaag += D[i]-max(D[i2],max(D[i3],D[i4])); 
    }

	return newaag;
}


void ComputeBounds(int D[], int i, int * UB_in, int * LB_in){
  int and_diffs = D[i-t1] + D[i-t2] + D[i-t3] + D[i-t4];
  *UB_in = min(w,D[i-t0] + and_diffs +D[i-t5]) ;
  *LB_in = (max(D[i-t0],D[i-t5]) - min(D[i-t0],D[i-t5])) - and_diffs;
  *LB_in = max(0, *LB_in);
}

void printD(int D[], int i) 
// print out values j such that 0<=j<=i and D[j]>0, with multiplicity if >1
// e.g. 121.4 means D[121]=4.
{ int j;
  printf("Difference pattern: ");
  for (j=0;j<=i;j++) 
    if (D[j]!=0) { 
			printf("%3d",j);
			if (D[j]==1) printf(" ");
			else printf(".%d ",D[j]);
      }
  printf("\n");
}
