#include "type.h"
#include "wrapper_constants.h"

void dataproc_avg(u32 img_set[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR], float result_set[MAX_LATENCY-1]);
void dataproc_sum(u32 img_set[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR], int result_set[MAX_LATENCY-1]);