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
#include "/opt/xilinx/Xilinx_Vivado_vitis_2019.2/Vivado/2019.2/include/gmp.h"
#include "/opt/xilinx/Xilinx_Vivado_vitis_2019.2/Vivado/2019.2/include/mpfr.h"

#include <ap_int.h>
#include <string.h>
#include "../include/xi_deconv_config.h"
#include "xi_deconv.hpp"


int XiDeconvTop(INTYPE_FLOAT* deconvIN, WTYPE* deconvWT, WTYPE* deconvBias, OUTIDTYPE* deconvIDout, int *scalar_deconv_args)
{
// Not previously defined in xi_deconv_top.cpp, so these depth values can be played with. Goal is to accomodate HLS synthesis.

//#if(__SDSOC==0)
#pragma HLS interface ap_bus port=deconvIN              depth=115200 // Could be 1200 like convolution
#pragma HLS interface ap_bus port=deconvWT              depth=115200 // Could be 500 like convolution
#pragma HLS interface ap_bus port=deconvBias            depth=115200 // Could be 1200 like convolution
#pragma HLS interface ap_bus port=deconvIDout           depth=115200 // Could be 1200 like convolution
#pragma HLS interface ap_bus port=scalar_deconv_args    depth=128    // Depth matches convolution
//#endif
	
	xideconv(deconvIN, deconvWT, deconvBias, deconvIDout, scalar_deconv_args);

	return 0;
}

