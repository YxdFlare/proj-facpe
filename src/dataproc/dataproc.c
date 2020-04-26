#include "type.h"
#include "common.h"
#include "wrapper_constants.h"
#include <stdio.h>

#define RISING_WEIGHT 1
#define FALING_WEIGHT 2

void dataproc_avg(u32 img_set[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR], float result_set[MAX_LATENCY-1])
{ 
  #ifndef __SYNTHESIS__
    printf("\nIN DATAPROC:\n================\n");
  #endif
  float pwr = 0;
  int pix_idx = 0;
  for(int j = 0; j < MAX_LATENCY -1; j++) {
    #ifndef __SYNTHESIS__
      printf("%d->%d\n",j,j+1);
    #endif
    pwr = 0;
    for(int x = 0; x < SIZE; x++) {
      for(int y = 0; y < SIZE; y++) {
        pix_idx = y*CH_NBR+x*SIZE*CH_NBR+j*SIZE*SIZE*CH_NBR;
        pwr += (RISING_WEIGHT * img_set[pix_idx+1]) + (FALING_WEIGHT * img_set[pix_idx+2]);
        #ifndef __SYNTHESIS__
          printf("[%3d](%d,%d,%d,)\t",pix_idx,img_set[pix_idx],img_set[pix_idx+1],img_set[pix_idx+2]);
        #endif
      }
      #ifndef __SYNTHESIS__
        printf("\n");
      #endif      
    }
    pwr /= (SIZE*SIZE);
    // #ifndef __SYNTHESIS__
    //   printf("Analyzed Pwr : %5.3f\n",pwr);
    // #endif
    result_set[j] = pwr;
  }
}

