#include <stdio.h>

const int t0=17, t1=18, t2=21, t3=31, t4=67, t5=89; // tap positions
#define n 89                                        // register size
#define w 64                                        // word size
#define c 16                                        // round size
#define md6gamma 5

#define min(a,b) ((a)<(b)? (a) : (b))
#define max(a,b) ((a)>(b)? (a) : (b))

#define TRUE (1)
#define FALSE (0)

int i2, i3, i4;



             //0, 1, 2,  3,  4,  5,  6,  7,
int DAMP[] = { 0, 7, 12, 15, 16, 15, 12, 7, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1,   1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1,   1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1,   1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };


int amp(int diff){

  return DAMP[diff];
}


int ComputeAndGatesActivated(int D[], int i){

		int newaag = amp(D[i]);

		// if i>=(t4-t2): then there is enough room for t4,t3
		if (i>=(t4-t2)) {
			i3 = i-(t3-t2);
			i4 = i-(t4-t2);  

      if (FALSE){
			  if (D[i] > max(D[i3], amp(D[i4])))         
				  newaag += D[i]-(max(D[i3], amp(D[i4]))); 
      } else {
			  if (D[i] > max(D[i3],D[i4]))         
				  newaag += D[i]-(max(D[i3],D[i4])); 
      }
		}


    // count new active AND gates that are within our
    // window, and which have their right input
    // on position i, and which weren't counted before
    if (FALSE) {
      if (i>=(t4-t3) && D[i]> amp(D[i-(t4-t3)]) ) 
			  newaag += D[i]-amp(D[i-(t4-t3)]);
    } else {
      if (i>=(t4-t3) && D[i]>D[i-(t4-t3)]) 
			  newaag += D[i]-D[i-(t4-t3)];
    }

		// all diff tap positions have enough room to exist all at once
    if (i>=(t4-t1)) { 
      i2 = i-(t2-t1);
      i3 = i-(t3-t1);
      i4 = i-(t4-t1);

      if (FALSE) {
        if (D[i] > max(D[i2],max(D[i3], amp(D[i4]) )))      
          newaag += D[i]-max(D[i2], max(D[i3], amp(D[i4]) )); 
      } else {
        if (D[i] > max(D[i2],max(D[i3],D[i4])))      
          newaag += D[i]-max(D[i2],max(D[i3],D[i4])); 
      }
    }

	return newaag;
}


void ComputeBounds(int D[], int i, int * UB_in, int * LB_in){
  int max_t1 = 0;
  int min_t1 = 0;
  if (TRUE){
    max_t1 = D[i-t1]*7;
    min_t1 = amp(D[i-t1]);
  } else {
    max_t1 = D[i-t1];
    min_t1 = D[i-t1];
  }

  int and_diffs =  D[i-t2] + D[i-t3] + D[i-t4];
  
  //int and_diffs = D[i-t1] + D[i-t2] + D[i-t3] + D[i-t4];
  *UB_in = min(w, D[i-t0] + (and_diffs + max_t1)  +D[i-t5]) ;
  *LB_in = (max(D[i-t0],D[i-t5]) - min(D[i-t0],D[i-t5])) - (and_diffs + min_t1);
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
