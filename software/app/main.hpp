#include <ap_int.h>

#if PORT_BITWIDTH_64BIT
#define GMEM_INPUTTYPE       unsigned long long int
#define GMEM_INTYPE_OTHER    ap_uint<64>
#define GMEM_WEIGHTTYPE      ap_uint<64>
#define GMEM_OUTTYPE         ap_uint<64>
#define GMEM_MAXPOOLTYPE     ap_uint<128>
#define GMEM_INTYPE_FC       ap_uint<128>
#define GMEM_BIASTYPE        unsigned long long int
#else
#define GMEM_INPUTTYPE       unsigned long long int
#define GMEM_INTYPE_OTHER    ap_uint<128>
#define GMEM_WEIGHTTYPE      ap_uint<128>
#define GMEM_OUTTYPE         ap_uint<128>
#define GMEM_MAXPOOLTYPE     ap_uint<128>
#define GMEM_INTYPE_FC       ap_uint<128>
#define GMEM_BIASTYPE        unsigned long long int
#endif  //#if PORT_BITWIDTH_64BIT

#define CONV_FLAG 0		
#define POOL_FLAG 1		
#define DECONV_FLAG 2		
#define __CONV_ENABLE__ 1		
#define __POOL_ENABLE__ 1		
#define __DECONV_ENABLE__ 0		

#define CHAR_TYPE     char
#define SHORT_TYPE    short
#define INT_TYPE      int


int DnnWrapper( 		
ap_uint<128>*, ap_uint<128>*, 
ap_uint<128>*, ap_uint<128>*, 
ap_uint<128>*, 
ap_uint<128>*, 
unsigned long long*, 
unsigned long long*, 
unsigned long long*, unsigned long long*, 
unsigned long long*, unsigned long long*, 
int*, 
ap_uint<128>*, ap_uint<128>*, 
ap_uint<128>*, ap_uint<128>*, 
ap_uint<128>*, 
int*, 
int, 
int, unsigned int, unsigned int, int, unsigned int*, unsigned int*, float*
);