#include "type.h"

u32 list[10] = {
    0x72160722,
    0x00000000,
    0x12345678,
    0xdeadbeef,
    0x00c0ffee,
    0xfaceface,
    0x1a2b3c4d,
    0x14789523,
    0x19970119,
    0x00001949,
};

u32 test_inputgen(int index)
{
  return list[index % 10];
}