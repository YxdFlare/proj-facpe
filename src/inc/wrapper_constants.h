#ifndef WRAPPER_CONST_H
#define WRAPPER_CONST_H

// address mapping
#define OPCODE_BASE    0x00000000
#define STATE_BASE     0x00000001
#define CONFIG_BASE    0x00000002
#define DUT_IN_BASE    0x00000010
#define DUT_OUT_BASE   0x00000018
#define DFT_OUT_BASE   0x00000020
#define TEST_IN_BASE   0xFF000000
#define TEST_OUT_BASE  0xFF000001
#define INVALID_ADDR   0xFFFFFFFF
#define NULL_ADDR      0x12345678

// state def 
#define IDLE           0
#define INPUT_FLATTEN  1
#define INPUT_DUT      2
#define INPUT_RDY      3
#define OUTPUT_WAIT    4
#define OUTPUT_VAL     5
#define OUTPUT_PACK    6
#define SCAN_PREP      7
#define SCAN           8
#define SCAN_RD        9
#define TICK           10
#define INVALID_STATE  0xffffffff

// state register def
#define WRAPPER_FSM_CS 0x00F
#define DUT_CM_ACK 0x010
#define DUT_OP_CM  0x020
#define DUT_OP_ACK 0x040
#define DUT_VAL_OP 0x080
#define DFT_CM_ACK 0x100
#define DFT_OP_CM  0x200
#define DFT_OP_ACK 0x400
#define DFT_VAL_OP 0x800

// opcode codebook
#define NONE  0
#define INPUT 1
#define RUN   2
#define ENDR  3
#define TEST  4
#define NEXT  5
#define ENDT  6

// scan chain parameters
#define DUMP_NBR 1
#define CHAIN_NBR 1

// DUT parameter
#define MAX_LATENCY 9
#define DUT_ILEN 32
#define DUT_OLEN 32
#define DUT_IACK 1
#define DFT_IACK 1

// DUFT operations
#define WRITE  0
#define READ   1

// encoder dimensions
#define CH_NBR 3
#define SIZE   6

// interface
#define IO_LATENCY 3

#endif