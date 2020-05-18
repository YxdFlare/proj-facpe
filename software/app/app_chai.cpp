#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#undef __ARM_NEON__
#undef __ARM_NEON
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#define __ARM_NEON__
#define __ARM_NEON

#include <iostream>
using namespace std;
using namespace cv;

#include "../interface/xi_interface.hpp"
#include "../interface/xi_readwrite_util.hpp"
#include "../checkers/checkers.hpp"

#include "../../src/inc/wrapper_constants.h"
#include "../../src/inc/top_constants.h"
#include "../../src/inc/type.h"

int dataproc_chai(char* dirpath, char* prototxt, char* caffemodel, unsigned int* img_path1, unsigned int* img_path2, float* final_results) {
    io_layer_info io_layer_info_ptr1;

    string start_layer = "";
    string end_layer = "";
   
    int numImg_to_process = 2;
    if (numImg_to_process != 2) {
      fprintf(stderr,"[ERROR] (dataproc_chai) Only support batch of 2\n");
      return -1;
    }
    bool is_first_layer_1 = 1;
    
    void *chai_handle1 = xiInit(dirpath, prototxt, caffemodel, &io_layer_info_ptr1, 
				numImg_to_process, is_first_layer_1, start_layer, end_layer);
    cerr << "[INFO] (dataproc_chai) Handle1 Initialized at : " << hex << chai_handle1 << endl;

   

    // normalized unpacked input buffer
    vector<void *> normalizeInput;
    IO_DATA_TYPE npk_img0[6*6*3];
    IO_DATA_TYPE npk_img1[6*6*3];
    normalizeInput.push_back((void*)npk_img0);
    normalizeInput.push_back((void*)npk_img1);

    // void *ptr;
    // vector<void *> normalizeInput;
    // for (int batch_id = 0; batch_id < numImg_to_process; batch_id++) {
    //   //void *ptr = (int8_t *)malloc(resize_h*resize_w*3*sizeof(int8_t));
    //   ptr = malloc(6*6*3*sizeof(int8_t));
    //  	normalizeInput.push_back(ptr);
    // }

    int status = input_u32(normalizeInput,img_path1,img_path2, 
    				numImg_to_process, io_layer_info_ptr1);

    if (status == -1) {
      fprintf(stderr, "[ERROR] IMAGE READ FAIL\n");
      return -1;
    } else {
      fprintf(stderr, "[INFO] IMAGE READ SUCCESS\n");
    }    
        
    //# Create input/output Buffers    
    vector<void *> input;
    int in_size = io_layer_info_ptr1.inlayer_sizebytes;
    int in_bufs = io_layer_info_ptr1.num_in_bufs;
    int i;
    #ifdef USER_DEBUG
      fprintf(stderr, "[INFO] input buffer list\n");
    #endif
    IO_DATA_TYPE pkd_imgs[in_bufs][in_size];
    for(i = 0; i < in_bufs; i++) {
      input.push_back((void*)pkd_imgs[i]);
      #ifdef USER_DEBUG
        fprintf(stderr, "\t buffer at %p (size %i)\n",(void*)pkd_imgs[i],in_size);
      #endif
    }
        
    vector<void *> output;
    int out_size = io_layer_info_ptr1.outlayer_sizebytes;
    int out_bufs = io_layer_info_ptr1.num_out_bufs;
    #ifdef USER_DEBUG
      fprintf(stderr, "[INFO] input buffer list\n");
    #endif
    IO_DATA_TYPE pkd_outs[out_bufs][out_size];
    for(i = 0; i < out_bufs; i++){
      output.push_back((void*)pkd_outs[i]);
      #ifdef USER_DEBUG
        fprintf(stderr, "\t buffer at %p (size %i)\n",(void*)pkd_outs[i],out_size);
      #endif
    }

    cerr << "[INFO] (dataproc_chai) Output Kernel Type : " << io_layer_info_ptr1.out_kerType << endl;

    fprintf(stderr,"[CHAI] (xiInputRead1) I@{0x%x},O@{0x%x}\n",normalizeInput[0],input[0]);
      xiInputRead(normalizeInput, input, numImg_to_process, io_layer_info_ptr1);
    fprintf(stderr,"[CHAI] (xiInputRead1) COMPLETE\n");

    fprintf(stderr,"[CHAI] (xiExec1) I@{0x%x},O@{0x%x}\n",input[0],output[0]); 

      u8*    ucptr;
        fprintf(stderr,"\toutput before exec\n");
        for(i = 0; i < io_layer_info_ptr1.num_out_bufs; i++){
          ucptr = (u8*)output[i];
          fprintf(stderr,"\tA : %p %p %p %p %p %p %p %p\n",ucptr,ucptr+1,ucptr+2,ucptr+3,ucptr+4,ucptr+5,ucptr+6,ucptr+7);
          fprintf(stderr,"\tD : %2x             %2x             %2x             %2x             %2x             %2x             %2x             %2x \n",
                  ucptr[0],ucptr[1],ucptr[2],ucptr[3],ucptr[4],ucptr[5],ucptr[6],ucptr[7]);
        }
  
      xiExec(chai_handle1, input, output,CHAI,0,0,0,0,0,0);

        fprintf(stderr,"\toutput after exec\n");
        for(i = 0; i < io_layer_info_ptr1.num_out_bufs; i++){
          ucptr = (u8*)output[i];
          fprintf(stderr,"\tA : %p %p %p %p %p %p %p %p\n",ucptr,ucptr+1,ucptr+2,ucptr+3,ucptr+4,ucptr+5,ucptr+6,ucptr+7);
          fprintf(stderr,"\tD : %2x             %2x             %2x             %2x             %2x             %2x             %2x             %2x \n",
                  ucptr[0],ucptr[1],ucptr[2],ucptr[3],ucptr[4],ucptr[5],ucptr[6],ucptr[7]);
        }

    fprintf(stderr,"[CHAI] (xiExec1) COMPLETE\n\n");

    fprintf(stderr, "[INFO] (dataproc_chai) FINISHED EXECUTION\n");
    fflush(stderr);    

    int unpack_out_size = io_layer_info_ptr1.outlayer_sizebytes;
    fprintf(stderr, "[INFO] (dataproc_chai) unpacked output size %i\n",unpack_out_size);
    //# Create memory for unpack output data
    vector<void *> unpack_output;
    for(int batch_id = 0; batch_id < numImg_to_process; batch_id++)
    {
      //void *ptr = malloc(unpack_out_size);
      void* ptr = (void*)(final_results + batch_id);
      unpack_output.push_back(ptr);
    }
    //# Loading required params for unpack function
    kernel_type_e out_kerType = io_layer_info_ptr1.out_kerType;
    int out_layer_size = io_layer_info_ptr1.out_size;
    fprintf(stderr, "[INFO] (dataproc_chai) output layer size %i\n",out_layer_size);
    //# unpacks the output data
    xiUnpackOutput(output, unpack_output, out_kerType, out_layer_size, numImg_to_process);
    //# Write the output data to txt file
    //outputWrite(dirpath, img_path1, unpack_output, numImg_to_process, io_layer_info_ptr1, 0);
    
    xiRelease(chai_handle1); //# Release before exiting application
    return 0;
}

