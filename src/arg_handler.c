#define n 89                                        // register size
#define c 16                                        // round size


/*  
 *  minSteps  - the number of steps that we start our search at.
 *  maxSteps  - when we exceed this number of steps we stop our search
 *  minAAGs   - the Active And Gate limit we start our search at
 *  maxAAGs   - the maximum allowed Active And Gate limit in our search
 *  stepIncr  - the number of steps we increment the step limit by on each interation
 *  extraAAGs - this is used if we counted extra AAGs somewhere else and want to add them to total found here
 *  tp        - decides if we are only going to look at trivial patterns
 *  ntp       - decides if we are only going to look at non-trivial patterns
 *  sr        - skip first rotation if sr == 1
 *  p1        - selects a some position as the first differential position
 *  p2        - selects a some position as the second differential position
 *  jsonOut   - turns on json output, useful for easy parsing.
 */
struct arguments {
  int minSteps, maxSteps, minAAGs, maxAAGs, stepIncr, extraAAGs, tp, ntp, sr, p1, p2, jsonOut;
};

static struct argp_option options[] = {
  {"minSteps",  'a', "Minimum steps",      0,  "The minimum number of steps to search." },
  {"maxSteps",  'b', "Maximum steps",      0,  "The maximum number of steps to search." },
  {"minAAGs",   'g', "Minimum AAGs",      0,  "The minimum number of Active And Gates to search." },
  {"maxAAGs",   'h', "Maximum AAGs",      0,  "The minimum number of Active And Gates to search." },
  {"step-increment",
                'i', "increment",      0,  "The value by which the current step limit is incremented." },
  {"extraAAGs", 'e', "AAGs",            0,  "Increase the number of AAGs counted by this amount"},
  {"trivial-patterns", 
               't',      0,      0,  "Only search trivial patterns."},
  {"non-trivial-patterns",       
               'n',      0,      0,  "Only search non-trivial patterns."},
  {"skip-rotation",       
               'r',      0,      0,  "Do not compute the first rotation (89 steps)."},
  {"p1",       '1',  "DP1",      0,  "The first fixed differential position."},
  {"p2",       '2',  "DP2",      0,  "The second fixed differential position."},
  {"json-output",
               'j',      0,      0,  "Provide output in the json format for easy processing."},
{ 0 }
};


/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state){
 /* Get the input argument from argp_parse, which we
    know is a pointer to our arguments structure. */
 struct arguments *arguments = state->input;

 switch (key) {
   case 'a':
     arguments->minSteps = atoi ( arg );
     break;
   case 'b':
     arguments->maxSteps = atoi ( arg );
     break;
   case 'g':
     arguments->minAAGs = atoi ( arg );
     break;
   case 'h':
     arguments->maxAAGs = atoi ( arg );
     break;
   case 'i':
     arguments->stepIncr = atoi ( arg );
     break;
   case 'e':
     arguments->extraAAGs = atoi ( arg );
     break;
   case 't':
     arguments->tp = 1;
     break;
   case 'n':
     arguments->ntp = 1;
     break;
   case 'r':
     arguments->sr = 1;
     break;
   case '1':
     arguments->p1 = atoi ( arg );
     break;
   case '2':
     arguments->p2 = atoi ( arg );
     break;
   case 'j':
     arguments->jsonOut = 1;
     break;

   default:
     return ARGP_ERR_UNKNOWN;
   }
 return 0;
}

static error_t verifyArgs (struct arguments *arguments){
  if (arguments->tp == 1 && arguments->ntp == 1){
    printf("ERROR bad arguments! TP and NTP are exclusive both can not be on at" 
            " the same time.\n");
    return 1;
  }

  if (arguments->p1 == -1 && arguments->p2 != -1){
    printf("ERROR bad arguments! If a value for p2 is choosen a" 
            " value for p1 must be choosen as well.\n");
    return 1;
  }

  if (arguments->sr == 1 && arguments->p1 != -1 && arguments->p1 < n){
    printf("ERROR bad arguments! If the first rotation is skipped not value"
            " less than the first rotation can be choosen for a fixed"
            " differential position.\n");
    return 1;
  }

  if (arguments->sr == 1 && arguments->tp == -1){
    printf("ERROR bad arguments! Can not skip the first rotation is we are"
            " searching for trivial patterns.\n");
    return 1;
  }


  return 0;
}

void populateDefaultArgValues(struct arguments *arguments){
  /* Default values. */
  arguments->minSteps = 0;
  arguments->maxSteps = 18*c;
  arguments->minAAGs = 2;
  arguments->maxAAGs = 70;
  arguments->stepIncr = c;
  arguments->extraAAGs = 0;

  arguments->ntp = 0;
  arguments->tp = 0;
  arguments->sr = 0;
  arguments->p1 = -1;
  arguments->p2 = -1;
  arguments->jsonOut = 0;
}

void printArgValues(struct arguments *arguments){
  printf (
    "Searching for differential paths with the following options and constraints:\n"
    "\tminSteps = %d, maxSteps = %d, minAAGs = %d, maxAAGs = %d, step increment = %d\n"
    "\tntp = %s, tp = %s, skip-Rotation = %s, jsonOutput = %s\n",
    arguments->minSteps,
    arguments->maxSteps,
    arguments->minAAGs,
    arguments->maxAAGs,
    arguments->stepIncr,
    arguments->ntp ? "yes" : "no",
    arguments->tp ? "yes" : "no",
    arguments->sr ? "yes" : "no",
    arguments->jsonOut ? "yes" : "no"  );
}

