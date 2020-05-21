# FPGA Acceleration of CNN-based Power Estimation
This is the code repository for Yixiao Du (yd383@cornell.edu) 's M.Eng. project at Cornell University.

## Table of Contents
  __dsign__ : hardware source codes of CHaiDNN

  __model__  : quantized models used in this project

  __script__ : .tcl scripts used to run the simulation

  __software__ : software application of DUFT and CHaiDNN

  __src__ : source codes beside CHaiDNN

## Reproduction of Results
All the source code of this project could be found here. Two quantized models are also included in the model directory. To reproduce the results mentioned in, the repository needs to be cloned first. I use zhang-x1.ece.cornell.edu server for simulation. The commands used for this step is shown below.

    ssh <netid>@zhang-x1.ece.cornell.edu
  
    $ git clone https://github.com/YxdFlare/proj-facpe.git

Set up the Vivado 2019.2 environment before running simulation of the system:

    $ source /opt/xilinx/Xilinx_Vivado_vitis_2019.2/Vivado/2019.2/settings64.sh

    $ export XILINXD_LICENSE_FILE=/scratch/xilinx/sdaccel-zhang-x1.lic

Make a build-vivado folder under the root of the project repository:

    $ cd ~/proj-facpe/

    $ mkdir build-vivado

Switch to that build directory and run the simulation. By default the simulation is performed on the whole system including CHaiDNN:

    $ cd build-vivado

    $ vivado_hls -f ../scripts/vivado_run_CHaiDNN.tcl

DUFT-only mode and debug mode are also supported. To run simulation in this mode, the vivado_run_CHaiDNN.tcl script need to be modified slightly:

    9	set DEBUG “”

    10	#set DEBUG “-DUSER_DEBUG”		uncomment this line for debug mode, change to -DDUFT_ONLY for DUFT-only mode

The debug mode will only run the first two images encoded from the first test item, but the testbench will print detailed information such as the address of the variables. The DUFT-only mode is used to test the functionality of the DUFT and the encoder.
