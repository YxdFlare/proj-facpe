#include "type.h"

void array_rst(u32* arr, u32 X, u32 Y, u32 Z, u32 C);
int array_teq(u32* ,u32 ,u32* ,u32 ); 
int array_teq2d(u32* arr1, u32 X1, u32 Y1, u32* arr2, u32 X2, u32 Y2);
int array_teq3d(u32* arr1, u32 X1, u32 Y1, u32 Z1, u32* arr2, u32 X2, u32 Y2, u32 Z2);
int waiting_timer(int* timer, int max);
void print_array(u32* arr, u32 len);
void print_array2d(u32* arr, u32 X, u32 Y);
void print_array3d(u32* arr, u32 X, u32 Y, u32 Z);
u32 get_bit(u32 data, int idx);