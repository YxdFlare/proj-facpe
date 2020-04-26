#include "type.h"
#include <stdio.h>

void array_rst(u32* arr, u32 X, u32 Y, u32 Z, u32 C)
{
  for(int i = 0; i < X; i++)
    for(int j = 0; j < Y; j++)
      for(int k = 0; k < Z; k++)
        for(int q = 0; q < C; q++)
          *(arr + q + k*C + j*C*Z + i*C*Z*Y) = 0;
}

int array_teq(u32* arr1, u32 len1, u32* arr2, u32 len2) 
{
  if(len1 != len2)
    return 0;
  else {
    int eq = 1;
    for(int i = 0; i < len1; i++)
      eq = eq && (arr1[i] == arr2[i]);
    return eq; 
  }
}

int array_teq2d(u32* arr1, u32 X1, u32 Y1, u32* arr2, u32 X2, u32 Y2)
{
  if(X1 != X2)
    return 0;
  else {
    int eq = 1;
    for(int i = 0; i < X1; i++)
      eq = eq && array_teq(arr1,Y1,arr2,Y2);
    return eq;
  }
}

int array_teq3d(u32* arr1, u32 X1, u32 Y1, u32 Z1, u32* arr2, u32 X2, u32 Y2, u32 Z2)
{
  if(X1 != X2)
    return 0;
  else {
    int eq = 1;
    for(int i = 0; i < X1; i++)
      eq = eq && array_teq2d(arr1,Y1,Z1,arr2,Y2,Z2);
    return eq;
  }
}

void print_array(u32* arr, u32 len)
{
  for(int i = 0; i < len; i++)
    printf("%d\t",arr[i]);
  printf("\n");
}

void print_array2d(u32* arr, u32 X, u32 Y)
{
  for(int i = 0; i < X; i++) {
    for(int j = 0; j < Y; j++) {
      printf("%d\t",*(arr + j + i*Y));
    }
    printf("\n");
  }
}

void print_array3d(u32* arr, u32 X, u32 Y, u32 Z)
{
  for(int i = 0; i < X; i++) {
    for(int j = 0; j < Y; j++) {
      printf("(");
      for(int k = 0; k < Z; k++) {
        printf("%d,",*(arr + k + j*Z + i*Y*Z));
      }
      printf(")\t");
    }
    printf("\n");
  }
}

u32 get_bit(u32 data, int idx)
{
  u32 bit;
  bit = data >> idx;
  bit = bit & 1;
  return bit;
}