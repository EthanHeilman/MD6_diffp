#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <md6parts.c>

#define min(a,b) ((a)<(b)? (a) : (b))
#define max(a,b) ((a)>(b)? (a) : (b))

int successes = 0;

int computeExpectedAAGsInD(int D[], int dLen){
  int nAAGs = 0;

  int i;
  for ( i = t4; i < dLen; i++ ){
    int i1 = D[i-t4];
    int i2 = D[i-t3];
    int i3 = D[i-t2];
    int i4 = D[i-t1];
    
    nAAGs += max(max(i1, i2), max(i3, i4));
  }

  return nAAGs;
}

int testAndGatesComputed(int D[], int testRange){
    int nAAGs = 0;
    int i;
    for ( i = 0; i < testRange; i++ ){
      nAAGs += ComputeAndGatesActivated(D, i);
    }
    
    return nAAGs;
}

void testAAGCounter(){
    printf("Testing md6parts.ComputeAndGatesActivated ...\n");
    
    int dLen = n*3;
    int D[dLen];
    memset(D, 0, (sizeof(int) * dLen));

    int testRange = n*2;

    printf("\tTesting zero difference... ");

      int actualAAGs = testAndGatesComputed(D, dLen);
      int expectedAAGs = 0;

      assert(actualAAGs == expectedAAGs);

    printf("success!\n");
    successes++;

    printf("\tTesting single difference... "); 
      
      int diffPos1, diffPos2, diffPos3, diffPos4;

      for ( diffPos1 = 0; diffPos1 < testRange; diffPos1++ ){
        memset(D, 0, (sizeof(int) * dLen));
        D[diffPos1] = 1;

        expectedAAGs = computeExpectedAAGsInD(D, dLen);
        actualAAGs = testAndGatesComputed(D, dLen);

        assert(expectedAAGs == actualAAGs);
      }

    printf("success!\n");
    successes++;

    printf("\tTesting Two differences... "); 

      for ( diffPos1 = 0; diffPos1 < testRange-1; diffPos1++ ){
        for ( diffPos2 = diffPos1+1; diffPos2 < testRange; diffPos2++ ){

          memset(D, 0, (sizeof(int) * dLen));
          D[diffPos1] = 1;
          D[diffPos2] = 2;

          expectedAAGs = computeExpectedAAGsInD(D, dLen);
          actualAAGs = testAndGatesComputed(D, dLen);

          assert(expectedAAGs == actualAAGs);
        }
      }
    printf("success!\n");
    successes++;

    printf("\tTesting Three differences... "); 

      for ( diffPos1 = 0;               diffPos1 < testRange-2;   diffPos1++ ){
        for ( diffPos2 = diffPos1+1;    diffPos2 < testRange-1;   diffPos2++ ){
          for ( diffPos3 = diffPos2+1;  diffPos3 < testRange;     diffPos3++ ){
            memset(D, 0, (sizeof(int) * dLen));
            D[diffPos1] = 1;
            D[diffPos2] = 1;
            D[diffPos3] = 1;

            expectedAAGs = computeExpectedAAGsInD(D, dLen);
            actualAAGs = testAndGatesComputed(D, dLen);

            assert(expectedAAGs == actualAAGs);
          }
        }
      }
    printf("success!\n");
    successes++;


    printf("\tTesting four differences... "); 

      for ( diffPos1 = 0;               diffPos1 < testRange-3;   diffPos1++ ){
        for ( diffPos2 = diffPos1+1;    diffPos2 < testRange-2;   diffPos2++ ){
          for ( diffPos3 = diffPos2+1;  diffPos3 < testRange-1;     diffPos3++ ){
            for ( diffPos4 = diffPos3+1;  diffPos4 < testRange;     diffPos4++ ){
              memset(D, 0, (sizeof(int) * dLen));
              D[diffPos1] = 1;
              D[diffPos2] = 1;
              D[diffPos3] = 1;
              D[diffPos4] = 1;

              expectedAAGs = computeExpectedAAGsInD(D, dLen);
              actualAAGs = testAndGatesComputed(D, dLen);

              assert(expectedAAGs == actualAAGs);
            } 
          }
        }
      }
    printf("success!\n");
    successes++;

    printf("\tTesting four differences of different values... "); 

      for ( diffPos1 = 0;               diffPos1 < testRange-3;   diffPos1++ ){
        for ( diffPos2 = diffPos1+1;    diffPos2 < testRange-2;   diffPos2++ ){
          for ( diffPos3 = diffPos2+1;  diffPos3 < testRange-1;     diffPos3++ ){
            for ( diffPos4 = diffPos3+1;  diffPos4 < testRange-n;     diffPos4++ ){
              memset(D, 0, (sizeof(int) * dLen));
              D[diffPos1] = 4;
              D[diffPos2] = 3;
              D[diffPos3] = 2;
              D[diffPos4] = 1;

              expectedAAGs = computeExpectedAAGsInD(D, dLen);
              actualAAGs = testAndGatesComputed(D, dLen);

              assert(expectedAAGs == actualAAGs);
            } 
          }
        }
      }
    printf("success!\n");
    successes++;

    printf("\tTesting four differences of very different values... "); 

      for ( diffPos1 = 0;               diffPos1 < testRange-3;   diffPos1++ ){
        for ( diffPos2 = diffPos1+1;    diffPos2 < testRange-2;   diffPos2++ ){
          for ( diffPos3 = diffPos2+1;  diffPos3 < testRange-1;     diffPos3++ ){
            for ( diffPos4 = diffPos3+1;  diffPos4 < testRange-n;     diffPos4++ ){
              memset(D, 0, (sizeof(int) * dLen));
              D[diffPos1] = 64;
              D[diffPos2] = 3;
              D[diffPos3] = 5;
              D[diffPos4] = 7;

              expectedAAGs = computeExpectedAAGsInD(D, dLen);
              actualAAGs = testAndGatesComputed(D, dLen);

              assert(expectedAAGs == actualAAGs);
            } 
          }
        }
      }
    printf("success!\n");
    successes++;

  printf("\n");
}


void testComputeBounds(){

  printf("Testing md6parts.ComputeBounds...\n");
  int dLen = n*3;
  int D[1000];
  int sumD = 0;

  printf("\tTesting zero difference rotation... "); 

    int UB_in = -1;
    int LB_in = -1;
    memset(D, 0, dLen);

    ComputeBounds(D, n, &UB_in, &LB_in);
    assert(LB_in != -1);
    assert(UB_in != -1);

    assert(LB_in == 0);
    assert(UB_in == 0);

  printf("success!\n");
  successes++;

  printf("\tTesting one difference values... "); 

    UB_in = -1;
    LB_in = -1;

    memset(D, 0, dLen);
    D[n-t0] = 1;
    D[n-t1] = 1;
    D[n-t2] = 1;
    D[n-t3] = 1;
    D[n-t4] = 1;
    D[n-t5] = 1;

    ComputeBounds(D, n, &UB_in, &LB_in);
    assert(UB_in != -1);
    assert(LB_in != -1);

    sumD = D[n-t0] + D[n-t1] + D[n-t2] + D[n-t3] + D[n-t4] + D[n-t5];

    assert(LB_in == 0);
    assert(UB_in == sumD);
  
  printf("success!\n");
  successes++;

  printf("\tTesting difference end xor values... "); 

    UB_in = -1;
    LB_in = -1;

    memset(D, 0, dLen);
    D[n-t0] = 1;
    D[n-t1] = 0;
    D[n-t2] = 0;
    D[n-t3] = 0;
    D[n-t4] = 0;
    D[n-t5] = 2;

    ComputeBounds(D, n, &UB_in, &LB_in);
    assert(UB_in != -1);
    assert(LB_in != -1);

    sumD = D[n-t0] + D[n-t1] + D[n-t2] + D[n-t3] + D[n-t4] + D[n-t5];


    assert(LB_in == 1);
    assert(UB_in == sumD);
  
  printf("success!\n");
  successes++;

  printf("\tTesting difference center and values... "); 

    UB_in = -1;
    LB_in = -1;

    memset(D, 0, dLen);
    D[n-t0] = 0;
    D[n-t1] = 1;
    D[n-t2] = 2;
    D[n-t3] = 3;
    D[n-t4] = 4;
    D[n-t5] = 0;

    ComputeBounds(D, n, &UB_in, &LB_in);
    assert(UB_in != -1);
    assert(LB_in != -1);

    sumD = D[n-t0] + D[n-t1] + D[n-t2] + D[n-t3] + D[n-t4] + D[n-t5];


    assert(LB_in == 0);
    assert(UB_in == sumD);
  
  printf("success!\n");
  successes++;

  printf("\n");
}




int main(int argc, char *argv[]){

  testComputeBounds();
  testAAGCounter();

  printf("SUCCESS! No failed tests.\n");
  printf("Results: Tests Passed = %d, \t Tests Failed = 0 \n", successes);


  return 0;
}
