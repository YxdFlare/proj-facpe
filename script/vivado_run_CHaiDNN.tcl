#set part_name "xczu7ev-ffvc1156-2-e"
#set project_name "zcu104"
set part_name "xczu9eg-ffvb1156-2-e"
set project_name "zcu102"

open_project -reset $project_name

# Set the top-level function to be DnnWrapper from "design/wrapper/dnn_wrapper.cpp" 
set_top DnnWrapper

add_files -tb software/bufmgmt/xi_buf_mgmt.cpp -cflags "-std=c++0x \
-I./software/bufmgmt \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/checkers/checkers.cpp -cflags "-std=c++0x \
-I./software/checkers \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/common/xi_kernels.cpp -cflags "-std=c++0x \
-I./software/common \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/common/kernelinfo_class.cpp -cflags "-std=c++0x \
-I./software/common \
-I/software/scheduler \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/custom/custom_class.cpp -cflags "-std=c++0x \
-I./software/custom \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/imageread/xi_input_image.cpp -cflags "-std=c++0x \
-I./software/imageread -I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/include/xchange_structs.cpp -cflags "-std=c++0x \
-I./software/include -I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/init/xi_init.cpp -cflags "-std=c++0x \
-I./software/init -I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/interface/xi_interface.cpp -cflags "-std=c++0x \
-I./software/interface -I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I./software/xtract \
-I./software/scheduler \
-I./software/bufmgmt \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/interface/xi_readwrite_util.cpp -cflags "-std=c++0x \
-I./software/interface \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/scheduler/xi_perf_eval.cpp -cflags "-std=c++0x \
-I./software/scheduler \
-I./software/common \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/scheduler/xi_scheduler.cpp -cflags "-std=c++0x \
-I./software/scheduler \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/scheduler/xi_thread_routines.cpp -cflags "-std=c++0x \
-I./software/scheduler \
-I./software/include \
-I./software/common \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

add_files -tb software/scheduler/xi_utils.cpp -cflags "-std=c++0x \
-I./software/scheduler \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include \
-I/home/yd383/openblas/include"

set files [glob -directory "software/swkernels" "*.cpp"]
foreach file $files {
add_files -tb $file -cflags "-std=c++0x \
-I./software/include \
-I./software/swkernels \
-I/home/yd383/openblas/include \
-I/home/yd383/CHaiDNN-HLS/SD_Card/cblas/arm64 \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include"
}

set files [glob -directory "software/xtract" "*.cpp" "*.proto"]
foreach file $files {
add_files -tb $file -cflags "-std=c++0x \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf/include \
-I/home/yd383/openblas/include \
-I/home/yd383/CHaiDNN-HLS/SD_Card/cblas/arm64 \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include" 
}

add_files -tb software/xtract/caffe.pb.cc -cflags "-std=c++11 \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf/include \
-I/home/yd383/openblas/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include"

# This file has the include "xi_conv_config.h" that is needed to turn off __SDSOC flag 
add_files design/wrapper/dnn_wrapper.cpp -cflags "-std=c++0x -D__HW__ -D__CONV_ENABLE__ -g"

add_files design/conv/src/xi_convolution_top.cpp -cflags "-std=c++0x -D__HW__ -g -I./design/conv/include"

add_files design/pool/src/pooling_layer_dp_2xio_top.cpp -cflags "-std=c++0x -D__HW__ -g -I./design/pool/include"

add_files design/deconv/src/xi_deconv_top.cpp -cflags "-std=c++0x -D__HW__ -g -I./design/deconv/include"

# SPECIFY MODEL TO TEST HERE
add_files -tb "~/CHaiDNN-HLS/software/example/googlenet_ex.cpp" -cflags "-std=c++0x \
-I./software/interface \
-I./software/checkers \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/include \
-I/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86/include"

open_solution "solution3"
set_part $part_name

create_clock -period 11

#csim_design -ldflags "-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf -lcblas -lprotobuf"
csim_design -ldflags "-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf \
-L/home/yd383/openblas/lib \
-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86 \
-lopenblas
-lprotobuf"

csynth_design 

# CHOOSE EITHER VCD FILE GENERATION (1st option) or COSIM EXECUTION (2nd option)	
#cosim_design -ldflags "-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf -lcblas -lprotobuf" -trace_level port
cosim_design -ldflags "-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/Vivado/2018.2/data/simmodels/xsim/2018.2/lnx64/6.2.0/ext/protobuf \
-L/home/yd383/openblas/lib \
-L/opt/xilinx/Xilinx_SDx_2018.2_0614_1954/SDx/2018.2/target/x86 \
-lopenblas
-lprotobuf" -trace_level port
