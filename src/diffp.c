// diffp.c
// By Ron Rivest, Yiqun Yin, Yuncheng Lin, Emily Shen, and Ethan Heilman
// Jan 2, 2010
// 
// Program to lower bound work on differential attack via
// lower-bounding number of active AND gates there must be.

/* Changes from diff10.c to diffp.c - Ethan Heilman
 *
 * 1. Added test code to test aag counting and lowerbounding logic.
 * 2. Added ability to supply contraints on md6 input (trivial v. 
 *      non-trivial patterns).
 * 3. Added argument parser to increase ease of user and presentability.
 * 4. Refactored constants so that we can swap in md6 varients with an include. 
 * 5. Altered output to bring it more in line with the aims of change 2.
 * 6. Added options to allow users to more specifically state the space they 
 *      wish to search.
 * 7. Added make as code is now in more than one place ( diffp.c, test.c, 
 *      arg_handler.c )
 * 8. Added python script lb.py so that output files generated in by 
 *      diffp can be assembled into a full lowerbound.
 * 9. Added performance metrics such as numbe rof recursions and time taken for 
 *      a particular search.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <argp.h>
#include <assert.h>
#include <string.h>

//our other code files
#include <arg_handler.c>
#include <md6parts.c>

#define TRUE (1)

// Constants n, w, c, r, md6gamma are defined in md6parts.c

const char *argp_program_version = "diffp 0.9";
const char *argp_program_bug_address = "<ethan@geographicslab.org>";

static char doc[] = 
  "Computes the lower bound on the number of And Gates Activated (AAG) when"
   " running md6 under certain specified constraints (number of steps, types of"
   " input, etc). If options not supplied it uses default option.\n"
  "For example : diffp -a 96 -b 288 -i 1 -g 2 -h 70 -n -r";

static char args_doc[] = "";
struct arguments arguments;

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

// Bit difference counts and cumulative counts
int D[1000];        // D[i] = number of bit differences on word i; i = 0,1,...
int CD[1000];       // CD[i] = sum(0<=j<=i) D[j] (cumulative D)
int CDP[1000];      // CDP[i] = sum(0<=j<=i){ D[j]>0 return 1 }  Cumulative Differential Positions 

// Active AND gate counts and cumulative counts
int AAG[1000];      // AAG[i] = number of new active AND gates due to diffs in word i
int CAAG[1000];     // CAAG[i] = sum(0<=j<=i) AAG[j] (cumulative AAG)

int success;        // success=1 when legal pattern found
int rounds;         // number of rounds to be examined
int stepLimit;      // number of steps to be examined
int rotations;      // number of rotations to be examined
int maxnaag;        // budget for number of active AND gates during search

int minCDP = 4;     // minimum cumulative differental positions (used only by non-trivial patterns)
int maxDP = 3;      // maximum Differential Positions per round group (used only by trivial patterns)

unsigned long long int numOfRecursions = 0; // used for performance tracking


void Search(int i){ 
  int LB_in, UB_in;
  int newaag;

  numOfRecursions++; // We use the number of recursions to track performance.

  // D[0..i-1] and CD[0..i-1] are set; D[i] and CD[i] to be set here
  // AAG[0..i-1] and CAAG[0..i-1] are set; AAG[i] and CAAG[i] to be set here
  // debug:
  // printD(i-1);

  // stop search if more than maxnaag active AND gates are implied
  // by the current state of the search
  // This uses fact that each rotation must have at least two active
  // AND gates, since it must have at least one bit difference, and
  // that bit difference will cause at least two AND gates to be active
  // (The span of two of the four AND gates hitting the difference within
  // the rotation must be entirely within that rotation, and so can be 
  // accounted for as associated with that rotation.)


  if (i>0 && (maxnaag < CAAG[i-1] + 2*(rotations*n-i)/n ))
    return; 

  // stop search with success if we have reached the end of the
  // number of rounds allowed, and are within budget for active
  // AND gates
  if (i >= stepLimit){ 
      printD(D, i);  
      success = 1; 
      return; 
  }

  // compute lower bound and upper bound on *input* diffs to g for D[i]
  LB_in = 0; 
  UB_in = w;

  // If we are skipping the first rotation extend the bounding grace period to 
  // the second rotation. Otherwise 
  if ( ( arguments.sr == TRUE && i >= 2*n ) || 
       ( arguments.sr != TRUE && i >= n ) ){ 
   ComputeBounds(D, i, &UB_in, &LB_in);
   assert(UB_in <= w);
   assert(LB_in >= 0);
  }

  // now try all possibilities for D[i]
  for (D[i] = 0; success==0 && D[i]<=w; D[i]++) { 
      
    // For the sake of paralellism we may we to fix particular 
    // differential positions, using the commandline arguments p1 & p2
    if      ( arguments.p1  != -1 && i < arguments.p1   && D[i] != 0) 
      continue;  // Any values before the first DP must be zero 
    else if ( arguments.p1  != -1 && i == arguments.p1  && D[i] == 0) 
      continue;  // The first DP must be greater than zero
    else if ( arguments.p2 != -1 && i > arguments.p1 
                                     && i < arguments.p2  && D[i] != 0)
      continue;
    else if ( arguments.p2 != -1 && i == arguments.p2 && D[i] == 0) 
      continue;

    // can't get zero ouput diff without possibility of zero input diff
    if ( D[i]==0 && LB_in>0 ) continue;

    // value of D[i] can't be more than 4 times UB_in
    if ( (D[i]>0) && (D[i]>4*UB_in) ) continue;
    
    // can't get single output diff without possibility of input diff
    // of gamma or greater
    if (D[i]==1 && UB_in<md6gamma) continue;

    // Compute cumulative number CD[i] of differences up to posn i
    if (i==0) CD[i] = D[i];
    else CD[i] = CD[i-1] + D[i];


    newaag = 0;
    // calculate the cumulative differental positions compute cumulative active
    //   AND gates
    if ( ( arguments.sr == TRUE && i == n ) || 
         ( arguments.sr != TRUE && i == 0 ) ) {
         newaag = arguments.extraAAGs;
      if (D[i] == 0) {CDP[i] = 0;}
      else {          CDP[i] = 1;}
    } 
    else if (D[i] == 0) {CDP[i] = CDP[i-1];}
    else {               CDP[i] = CDP[i-1]+1;}

    newaag += ComputeAndGatesActivated(D, i);

    // compute cumulative active AND gates
    if ( ( arguments.sr == TRUE && i == n ) || 
         ( arguments.sr != TRUE && i == 0 ) )
      CAAG[i] = newaag;
    else
      CAAG[i] = CAAG[i-1] + newaag;

    if (CAAG[i] > maxnaag) break;
    
    // If we are searching for non-trivial patterns and the current pattern 
    // the current pattern becomes trivial, discard the pattern.
    if ( arguments.ntp == TRUE && 
         ( ( (i % n) == n-1 ) && ( CDP[i] - CDP[i - (n - 1)] < minCDP ) ) ) {
      return;
    }

    // If we are searching for trivial patterns and the current pattern 
    // ceases to be trivial (has more than the max differential positions in 
    // the first rotation), discard the pattern.
    if ( arguments.tp == TRUE && 
          ( i < n && CDP[i] > maxDP ) ) {
      return; } 

    // Always recurse within first rotation unless we are skipping the first 
    // rotation, in which case always recurse within the second rotation
    if ( ( arguments.sr == TRUE && i < ( 2*n ) ) || 
         ( arguments.sr != TRUE && i <     n )   || 
         ( CD[i] != CD[i-n] ) ){ // or if preceding rotation has some difference

      Search(i+1);
    }
  }
}

void ZeroArrays(){
    memset(CAAG, 0, (sizeof(int) * n));
    memset(AAG, 0, (sizeof(int) * n));
    memset(CDP, 0, (sizeof(int) * n));
    memset(CD, 0, (sizeof(int) * n));
    memset(D, 0, (sizeof(int) * n));  
}

int main(int argc, char *argv[])
{ 
  //deal with commandline arguments      
  populateDefaultArgValues(&arguments);

  argp_parse (&argp, argc, argv, 0, 0, &arguments);
  int error = verifyArgs(&arguments);
  if (error != 0)
    return error;
  
  printArgValues(&arguments);

  maxnaag = arguments.minAAGs;

  for (stepLimit = arguments.minSteps; 
        stepLimit <= arguments.maxSteps; 
        stepLimit+=arguments.stepIncr){

      success = 0;
      rounds = ((stepLimit)/c);
      rotations = rounds*c/n;

      // at least two AAGs per rotation
      while(success == 0) {

        printf("Rounds = %d, stepLimit = %d, rotations = %d, active AND gates = %d ... ", rounds, stepLimit, rotations,maxnaag);
        
        success = 0;
        numOfRecursions = 0;
        time_t start, stop;
         
        if ( arguments.sr == TRUE ) {

          // Since we are skipping the first rotation we pretend that the first
          // rotation had atleast 0 differences.      
          ZeroArrays();
          (void) time(&start);
            Search(n);
          (void) time(&stop);

        } else { 

          (void) time(&start);  
            Search(0);
          (void) time(&stop);

        }     

        fflush(stdout);
        
        if (success){ // print result

          printf("Successs! recursions = %llu, seconds = %li \n", numOfRecursions, (long int)(stop-start));
          if ( arguments.jsonOut == TRUE ) {
            printf("{'Success':'true', 'stepLimit':%d, 'rounds':%d, 'rotation':%d, 'AAG':%d, 'recursions':%llu, 'seconds':%li}\n", stepLimit, rounds, rotations, maxnaag, numOfRecursions, (long int)(stop-start));
          } 
          printf("\n");

        } else {

          printf("recursions = %llu, seconds = %li \n", numOfRecursions, (long int)(stop-start));
          
          if ( arguments.jsonOut == TRUE ) {
            printf("{'Success':'false', 'stepLimit':%d, 'rounds':%d, 'rotation':%d, 'AAG':%d, 'recursions':%llu, 'seconds':%li}\n", stepLimit, rounds, rotations, maxnaag, numOfRecursions, (long int)(stop-start));
          }         
          
          maxnaag++;
        }

        if (maxnaag > arguments.maxAAGs)
          return 0;
      }
  }
  return 0;
}

