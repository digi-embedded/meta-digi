/*
 * vnn_utils.h
 *
 * This provides helper functions for nbg-benchmark tool and wrappers around OpenVX lib
 * function. The function are mainly used for converting data types and loading
 * network binary graph.
 *
 * Author: Othmane AHL ZOUAOUI <othmane.ahlzouaoui@st.com> for STMicroelectronics.
 *
 * Copyright (c) 2023 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *
 *     http://www.opensource.org/licenses/BSD-3-Clause
 */

#ifndef _VNN_UTILS_H_
#define _VNN_UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <VX/vx_khr_cnn.h>
#include <VX/vx_khr_import_kernel.h>
#include <VX/vx_lib_extras.h>


#define MAX_NUM_DIMS 6
#define MAX_IO_NAME_LENGTH 128
#define VSI_NN_MAX_DEBUG_BUFFER_LEN 1024
#define ZEROS(a) memset(a,0,sizeof(a))
#define _CHECK_OBJ(ptr, label) \
do \
{ \
    if ((ptr) == NULL) \
    { \
        printf("create fail: file=%s,line = %d\n", __FILE__,__LINE__); \
        goto label; \
    } \
} while(0)

#define _CHECK_STATUS(status, label) \
do \
{ \
    if (status != VX_SUCCESS) \
    { \
        printf("process fail,status=%d, file=%s,line = %d\n",status,__FILE__, __LINE__); \
        goto label; \
    } \
} while(0)
typedef struct _inout_param
{
    vx_uint32    dim_count;
    vx_uint32    dim_size[MAX_NUM_DIMS];
    vx_enum      data_format;
    vx_enum      data_type;
    vx_enum      quan_format;
    vx_int8      fixed_pos;
    vx_float32   tf_scale;
    vx_int32     tf_zerop;
    vx_char      name[MAX_IO_NAME_LENGTH];
} inout_param;

typedef struct _inout_obj
{
    vx_enum data_type;//image or tensor
	union
	{
		vx_reference ref;
		vx_image  image;
		vx_tensor tensor;
		vx_array  array;
		vx_scalar scalar;
	}u;

}inout_obj;
typedef enum _file_type
{
	FILE_TYPE_TEXT,
	FILE_TYPE_BIN,
	FILE_TYPE_NOT_SUPPORT
}file_type;

vx_int8    vnn_Fp32toInt8(vx_float32 val, vx_int8 fixedPointPos);
vx_float32 vnn_Int8toFp32(vx_int8 val, vx_int8 fixedPointPos);
vx_uint8   vnn_Fp32toUint8(vx_float32 val, vx_int32 zeroPoint, vx_float32 scale);
vx_float32 vnn_Uint8toFp32(vx_uint8 val, vx_int32 zeroPoint, vx_float32 scale);
vx_float32 vnn_Int16toFp32(vx_int16 val, vx_int8 fixedPointPos);
vx_int16   vnn_Fp32toInt16(vx_float32 val, vx_int8 fixedPointPos);
vx_int16   vnn_Fp32toFp16(vx_float32 val);
vx_float32 vnn_Fp16toFp32(const vx_uint16 in);
vx_int8    vnn_Fp32toAsymInt8(vx_float32 val, vx_int32 zeroPoint, vx_float32 scale);
vx_float32 vnn_AsymInt8toFp32(vx_int8 val, vx_int32 zeroPoint, vx_float32 scale);
vx_int16   vnn_Fp32toAsymInt16(vx_float32 val, vx_int32 zeroPoint, vx_float32 scale);
vx_float32 vnn_AsymInt16toFp32(vx_int16 val, vx_int32 zeroPoint, vx_float32 scale);

vx_uint32  vnn_GetTypeSize(vx_enum format);
vx_uint32  vnn_GetTensorSize(vx_tensor tensor);
vx_uint32  vnn_GetTensorDims(vx_tensor tensor);
vx_uint32  vnn_GetTensorBufferSize(vx_tensor tensor);
vx_status  vnn_CopyTensorToData(vx_tensor tensor,void **buf);
vx_status  vnn_CopyTensorToFloat32Data(vx_tensor tensor,vx_float32 **buf);
vx_status  vnn_CopyDataToTensor(vx_tensor tensor,void *buf);
vx_status  vnn_CopyFloat32DataToTensor(vx_tensor tensor,vx_float32 *buf);
vx_status  vnn_LoadTensorFromFile(vx_tensor tensor,char *filename);
vx_status  vnn_SaveTensorToFileAsFloat32(vx_tensor tensor,char *filename);
vx_status  vnn_SaveTensorToFileAsBinary(vx_tensor tensor,char *filename);
vx_status  vnn_ShowTensorTop5(vx_tensor tensor);
vx_status  vnn_ShowTop5(inout_obj* obj);

vx_status  vnn_LoadDataFromFile(inout_obj* obj,char *filename);
vx_status  vnn_LoadTensorRandom(vx_tensor tensor);
vx_status  vnn_SaveDataToFile(inout_obj* obj,char *filename);
vx_status  vnn_QueryInputsAndOutputsParam(vx_kernel kernel,inout_param *input,vx_int32 *in_cnt,inout_param *output,vx_int32 *out_cnt);
vx_status  vnn_CreateObject(vx_context context, inout_param *param, inout_obj *obj);
vx_status  vnn_ReleaseObject(inout_obj *obj);
vx_bool    get_top(float *pfProb, float *pfMaxProb, uint32_t *pMaxClass, uint32_t outputCount, uint32_t topNum);
void       vnn_Log(const char *fmt, ...);
#endif
