#include "type.h"
#include "wrapper_constants.h"

// input: collected states of one transaction
/*  [j[k]] = [cycle_idx[dump_idx]]
    M = max_latency-1, P = dump_nbr-1;
    [0[0,1,...P]],[1[0,1,...P]],[2[0,1,...P]] ... [M[0,1,...P]]
*/

// output: encoded set of images
/*  [j[x[y[c]]]] = [cycle_idx[row_idx[col_idx[ch_idx]]]]
    M = max_latency-1; X = Y = size = ceil(sqrt(dump_nbr * 32));

      0       Y         0       Y         0       Y              0       Y   
      +-------+         +-------+         +-------+              +-------+  
     +-------+|        +-------+|        +-------+|             +-------+|  
  0 +-------+||     0 +-------+||     0 +-------+||    ...   0 +-------+||     
    |       ||+ 2     |       ||+ 2     |       ||+ 2          |       ||+ 2
    |       |+ 1      |       |+ 1      |       |+ 1           |       |+ 1  
  X +-------+ 0     X +-------+ 0     X +-------+ 0          X +-------+ 0 

        0                 1                 2          ...         M-1   
*/
void batch_encode(u32 collected_states[MAX_LATENCY*DUMP_NBR], u32 img_set[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR]);
