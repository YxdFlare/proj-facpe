#include "type.h"
#include "wrapper_constants.h"
#include "common.h"
#include <stdio.h>

// reform the current state into 1-channel square-image
// current_cycle[0],[1]...[dump_nbr-1] => img[x][y] = {0 or 1}
void reform(u32 current_cycle[DUMP_NBR], u32 img[SIZE*SIZE])
{
  int i,j;
  int bit_idx;
  int arr_idx;
  for(i = 0;i<SIZE;i++)
    for(j =0;j<SIZE;j++) {
      bit_idx = i * SIZE + j;
      arr_idx = bit_idx / 32;
      bit_idx = bit_idx % 32;
      if (arr_idx < DUMP_NBR)
        img[i * SIZE + j] = get_bit(current_cycle[arr_idx],bit_idx);
      else
        img[i * SIZE + j] = 0;
    }
}

void set_color(u32 pix[CH_NBR], u32 R, u32 G, u32 B)
{
  pix[0] = R;
  pix[1] = G;
  pix[2] = B;
}

//the first channel is 1 if the signal bit does not switch
//the second channel is 1 if the signal bit switches from 0 to 1
//the third channel is 1 if the signal bit switches from 1 to 0
void encode(u32 prev_cycle[SIZE*SIZE], u32 current_cycle[SIZE*SIZE], u32 diff_img[SIZE*SIZE*CH_NBR])
{
  int i,j,k;
  u32* pix;
  for(i = 0;i<SIZE;i++)
    for(j =0;j<SIZE;j++) {
        pix = diff_img + i * SIZE * CH_NBR + j * CH_NBR;
      if(prev_cycle[i*SIZE + j] == 1 && current_cycle[i*SIZE + j] == 0)
        set_color(pix,0,0,1);
      else if(prev_cycle[i*SIZE + j] == 0 && current_cycle[i*SIZE + j] == 1)
        set_color(pix,0,1,0);
      else
        set_color(pix,1,0,0);
    }
}

void batch_encode(u32 collected_states[MAX_LATENCY*DUMP_NBR], u32 img_set[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR])
{
  int cycle_idx;
  u32 prev[SIZE*SIZE];
  u32 current[SIZE*SIZE];
  u32* img_set_ptr;
  for(cycle_idx = 1; cycle_idx < MAX_LATENCY; cycle_idx++) {
    reform(&collected_states[cycle_idx-1],prev);
    reform(&collected_states[cycle_idx],current);
    img_set_ptr = img_set + (cycle_idx - 1) * SIZE * SIZE * 3;
    encode(prev,current,img_set_ptr);
  }
  #ifndef __SYNTHESIS__
    printf("\nIN ENCODE:\n================\n");
    for (int i = 0; i < MAX_LATENCY-1; i++) {
      printf("%d->%d\n",i,i+1);
      print_array3d(img_set + i*SIZE*SIZE*CH_NBR,SIZE,SIZE,CH_NBR);
    }            
  #endif
}