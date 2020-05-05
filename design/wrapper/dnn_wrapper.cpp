/*----------------------------------------------------
Copyright 2017 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
----------------------------------------------------*/

#include "/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include/gmp.h"
#include "/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include/mpfr.h"

#ifdef __SDSOC
#include "../conv/include/xi_conv_config.h"
#endif

#include "ap_int.h"

extern "C" {
  #include "../../src/top/top.h"
}

#include "../../src/inc/wrapper_constants.h"
#include "../../src/inc/top_constants.h"
#include "../../src/inc/type.h"

#if XI_64BIT_PORT_EN==1
#define KER_GMEM_INPUTTYPE		 unsigned long long int
#define KER_GMEM_INTYPE_OTHER    ap_uint<64>
#define KER_GMEM_WEIGHTTYPE      ap_uint<64>
#define KER_GMEM_BIASTYPE        unsigned long long int
#define KER_GMEM_OUTTYPE         ap_uint<64>
#define KER_GMEM_DECONVOUTTYPE   unsigned long long int
#define KER_SOFT_GMEM_TYPE       unsigned int
#define KER_GMEM_FC_INPUTTYPE	 ap_uint<128>
#define KER_GMEM_FC_OUTTYPE      ap_uint<128>
#else
#define KER_GMEM_INPUTTYPE		 unsigned long long int
#define KER_GMEM_INTYPE_OTHER    ap_uint<128>
#define KER_GMEM_WEIGHTTYPE      ap_uint<128>
#define KER_GMEM_BIASTYPE        unsigned long long int
#define KER_GMEM_OUTTYPE         ap_uint<128>
#define KER_GMEM_DECONVOUTTYPE   unsigned long long int
#define KER_SOFT_GMEM_TYPE       unsigned int
#define KER_GMEM_FC_INPUTTYPE	 ap_uint<128>
#define KER_GMEM_FC_OUTTYPE      ap_uint<128>
#endif

#define KER_GMEM_DECONVOUTTYPE   unsigned long long int

#if DECONV_FLOAT
#define KER_GMEM_DECONVINPTYPE   float
#else
#define KER_GMEM_DECONVINPTYPE   short
#endif

#define KER_GMEM_MAXPOOLTYPE   ap_uint<128>
#define KER_GMEM_POOLTYPE_WTS  ap_uint<128>

#define GMEM_RGBTYPE       unsigned long long int
#define GMEM_DECONVTYPE    unsigned long long int
#define GMEM_DISPLAYTYPE   unsigned long long int

#define CONV_FLAG 0
#define POOL_FLAG 1
#define DECONV_FLAG 2

#define __CONV_ENABLE__ 1
#define __POOL_ENABLE__ 1
#define __DECONV_ENABLE__ 0


#if __CONV_ENABLE__==1					
#include "../conv/src/xi_convolution_top.hpp" // Replaced function prototype below with header file

#endif//CONV KERNEL

#if __POOL_ENABLE__==1
int PoolTop(
			KER_GMEM_MAXPOOLTYPE *ind1, KER_GMEM_MAXPOOLTYPE *in2,
			KER_GMEM_MAXPOOLTYPE *out1, KER_GMEM_MAXPOOLTYPE *out2, 
			KER_GMEM_POOLTYPE_WTS *wts,
			int *scalar_pool_args
			);

#endif//POOL kernel

#if __DECONV_ENABLE__==1
#include "../deconv/src/xi_deconv_top.hpp" // Replaced function prototype below with header file

#endif//Deconv kernel

int DnnWrapper( 
#if __CONV_ENABLE__==1
        KER_GMEM_WEIGHTTYPE *weights1, KER_GMEM_WEIGHTTYPE *weights2,
#if (XI_KER_PROC==16 || (XI_64BIT_PORT_EN==1 && XI_KER_PROC==8))
        KER_GMEM_WEIGHTTYPE *weights3, KER_GMEM_WEIGHTTYPE *weights4,
#endif
				KER_GMEM_OUTTYPE *output1,
#if !XI_SINGLE_IO_PORT_EN
				KER_GMEM_OUTTYPE *output2,
#endif
				KER_GMEM_INTYPE_OTHER *input_other1,
#if !XI_SINGLE_IO_PORT_EN
				KER_GMEM_INTYPE_OTHER *input_other2,
#endif
				KER_GMEM_INPUTTYPE *input_1st, KER_GMEM_BIASTYPE *bias,
#if !XI_DISABLE_BN
				KER_GMEM_INPUTTYPE *input_norm2, KER_GMEM_INPUTTYPE *input_norm3,
#endif
				KER_GMEM_INPUTTYPE *istg_out1,
#if !XI_SINGLE_IO_PORT_EN
				KER_GMEM_INPUTTYPE *istg_out2,
#endif
				int *scalar_conv_args,
#endif //CONV kernel
#if __POOL_ENABLE__==1
        KER_GMEM_MAXPOOLTYPE *in1,KER_GMEM_MAXPOOLTYPE *in2,
				KER_GMEM_MAXPOOLTYPE *out1,KER_GMEM_MAXPOOLTYPE *out2,
				KER_GMEM_POOLTYPE_WTS *wts,
				int *scalar_pool_args,
#endif//POOL kernel
#if __DECONV_ENABLE__==1
        KER_GMEM_DECONVINPTYPE* deconvIN, KER_GMEM_DECONVINPTYPE* deconvWT, 
				KER_GMEM_DECONVINPTYPE* deconvBias, KER_GMEM_DECONVOUTTYPE* deconvIDout, int *scalar_deconv_args,
#endif//DECONV kernel
        int flag,
// duft top-level function arguments
int func, u32 addr, u32 data, int rd_wr,u32 encoded_imgset[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR], u32 dcs[MAX_LATENCY*DUMP_NBR], float final_results[MAX_LATENCY-1]        
        ) 
{
  if (func == CHAI) {
    #if __CONV_ENABLE__==1
      if (flag == CONV_FLAG) {
    #if __SDSOC
      #pragma SDS async(1)
    #endif

          XiConvolutionTop(weights1, weights2,
    #if (XI_KER_PROC==16 || (XI_64BIT_PORT_EN==1 && XI_KER_PROC==8))
                  weights3, weights4,
    #endif
                  output1,
    #if !XI_SINGLE_IO_PORT_EN
              output2,
    #endif
              input_other1,
    #if !XI_SINGLE_IO_PORT_EN
              input_other2,
    #endif
              input_1st, bias,
    #if !XI_DISABLE_BN
              input_norm2, input_norm3,
    #endif
              istg_out1,
    #if !XI_SINGLE_IO_PORT_EN
              istg_out2,
    #endif
              scalar_conv_args);
      }
    #endif//CONV kernel
    #if __POOL_ENABLE__==1
      if (flag == POOL_FLAG) {
    #if __SDSOC
      #pragma SDS async(2)
    #endif

        PoolTop(in1,in2,out1,out2, wts, scalar_pool_args);
      }
    #endif//POOL kernel
    #if __DECONV_ENABLE__==1
      if (flag == DECONV_FLAG) {
    #if __SDSOC
      #pragma SDS async(3)
    #endif

        XiDeconvTop(deconvIN, deconvWT, deconvBias, deconvIDout, scalar_deconv_args);
      }
    #endif//DECONV kernel

    return 0;
  }
  else {
    return top(func,addr,data,rd_wr,encoded_imgset,dcs,final_results);
  }
  
}
