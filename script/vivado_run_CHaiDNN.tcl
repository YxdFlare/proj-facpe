#set part_name "xczu7ev-ffvc1156-2-e"
#set project_name "zcu104"
set part_name "xczu9eg-ffvb1156-2-e"
set project_name "zcu102"

open_project -reset $project_name

# includes
set IDIRS "\
-I../software/bufmgmt \
-I../software/checkers \
-I../software/common \
-I../software/scheduler \
-I../software/custom \
-I../software/imageread \
-I../software/include \
-I../software/init \
-I../software/interface \
-I../software/xtract \
-I../lib/cblas/arm64 \
-I../lib/openblas/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include"

set LLIBS "\
-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf \
-L../lib/openblas/lib \
-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86 \
-lopenblas
-lprotobuf"

set CSTD "-std=c++0x"

# Set the top-level function to be DnnWrapper from "design/wrapper/dnn_wrapper.cpp" 
set_top DnnWrapper

add_files -tb ../software/bufmgmt/xi_buf_mgmt.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/checkers/checkers.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/common/xi_kernels.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/common/kernelinfo_class.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/custom/custom_class.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/imageread/xi_input_image.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/include/xchange_structs.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/init/xi_init.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/interface/xi_interface.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/interface/xi_readwrite_util.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/scheduler/xi_perf_eval.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/scheduler/xi_scheduler.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/scheduler/xi_thread_routines.cpp -cflags "$CSTD $IDIRS" 

add_files -tb ../software/scheduler/xi_utils.cpp -cflags "$CSTD $IDIRS" 

set files [glob -directory "../software/swkernels" "*.cpp"]
foreach file $files {
add_files -tb $file -cflags "$CSTD $IDIRS" 
}

set files [glob -directory "../software/xtract" "*.cpp" "*.proto"]
foreach file $files {
add_files -tb $file -cflags "$CSTD $IDIRS" 
}

add_files -tb ../software/xtract/caffe.pb.cc -cflags "$CSTD $IDIRS" 

# This file has the include "xi_conv_config.h" that is needed to turn off __SDSOC flag 
add_files ../design/wrapper/dnn_wrapper.cpp -cflags "$CSTD $IDIRS -D__HW__ -D__CONV_ENABLE__ -g"

add_files ../design/conv/src/xi_convolution_top.cpp -cflags "$CSTD $IDIRS -D__HW__ -g -I../design/conv/include"

add_files ../design/pool/src/pooling_layer_dp_2xio_top.cpp -cflags "$CSTD $IDIRS -D__HW__ -g -I./design/pool/include"

add_files ../design/deconv/src/xi_deconv_top.cpp -cflags "$CSTD $IDIRS -D__HW__ -g -I../design/deconv/include"

# SPECIFY MODEL TO TEST HERE
add_files -tb "../software/app/main.cpp" -cflags "$CSTD $IDIRS" 

open_solution "solution2"
set_part $part_name

create_clock -period 10

csim_design -ldflags $LLIBS

csynth_design 

cosim_design -ldflags $LLIBS -trace_level port
