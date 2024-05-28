/*
 * vnn_utils.cc
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

#include <stdarg.h>
#include "vnn_utils.h"

#ifdef WIN32
static vx_float32 round(vx_float32 x)
{
#if defined(_M_X64)
    return (vx_float32) _copysignf(floorf(fabsf(x) + 0.5f), x);
#else
    return (vx_float32) _copysign(floorf(fabsf(x) + 0.5f), x);
#endif
}
#endif

vx_uint32 vnn_GetTypeSize(vx_enum format)
{
    switch(format)
    {
        case VX_TYPE_INT8:
        case VX_TYPE_UINT8:
		case VX_DF_IMAGE_U8:
            return 1;
        case VX_TYPE_INT16:
        case VX_TYPE_UINT16:
		case VX_DF_IMAGE_S16:
		case VX_DF_IMAGE_U16:
            return 2;
        case VX_TYPE_INT32:
        case VX_TYPE_UINT32:
		case VX_DF_IMAGE_S32:
		case VX_DF_IMAGE_U32:
            return 4;
        case VX_TYPE_INT64:
        case VX_TYPE_UINT64:
            return 8;
        case VX_TYPE_FLOAT32:
		case VX_DF_IMAGE_F32:
            return 4;
        case VX_TYPE_FLOAT64:
            return 8;
        case VX_TYPE_ENUM:
            return 4;
        case VX_TYPE_FLOAT16:
            return 2;
		default:
			printf("Not support format:%d,line=%d\n",format,__LINE__);
    }
    return 0;
}

vx_int8 vnn_Fp32toInt8(vx_float32 val, vx_int8 fixedPointPos)
{
    vx_int8 result = 0;

    if (fixedPointPos > 0)
    {
        vx_int32 data = (vx_int32) round(val * (vx_float32)(1 << fixedPointPos));
        result = (vx_int8)((data > 127) ? 127 : (data < -128) ? -128 : data);

    }
    else
    {
        vx_int32 data = (vx_int32) round(val * (1.0f / (vx_float32)(1 << -fixedPointPos)));
        result = (vx_int8)((data > 127) ? 127 : (data < -128) ? -128 : data);
    }

    return result;
}

vx_float32 vnn_Int8toFp32(vx_int8 val, vx_int8 fixedPointPos)
{
    vx_float32 result = 0.0f;

    if (fixedPointPos > 0)
    {
        result = (vx_float32)val * (1.0f / ((vx_uint32) (1 << fixedPointPos)));
    }
    else
    {
        result = (vx_float32)val * ((vx_float32) (1 << -fixedPointPos));
    }

    return result;
}

vx_uint8 vnn_Fp32toUint8(vx_float32 val, vx_int32 zeroPoint, vx_float32 scale)
{
    vx_uint8 result = 0;
    vx_int32 data;

    data = (vx_int32) round((val / scale + (vx_uint8)zeroPoint));

    if (data > 255)
        data = 255;

    if (data < 0)
        data = 0;

    result = (vx_uint8)(data);

    return result;
}
vx_float32 vnn_Uint8toFp32(vx_uint8 val, vx_int32 zeroPoint, vx_float32 scale)
{
    vx_float32 result = 0.0f;

    result = (val - (vx_uint8)zeroPoint) * scale;
    return result;
}
vx_int8 vnn_Fp32toAsymInt8(vx_float32 val, vx_int32 zeroPoint, vx_float32 scale)
{
    vx_int8 result = 0;
    vx_int32 data;

    data = (vx_int32) round((val / scale + (vx_uint8)zeroPoint));

    if (data > 127)
        data = 127;

    if (data < -128)
        data = -128;

    result = (vx_int8)(data);

    return result;
}
vx_float32 vnn_AsymInt8toFp32(vx_int8 val, vx_int32 zeroPoint, vx_float32 scale)
{
    vx_float32 result = 0.0f;

    result = (val - (vx_int8)zeroPoint) * scale;
    return result;
}
vx_float32 vnn_Int16toFp32(vx_int16 val, vx_int8 fixedPointPos)
{
    vx_float32 result = 0.0f;
    vx_float32 value = val;
    if (fixedPointPos > 0)
    {
        result = value * (1.0f / ((vx_uint32) (1 << fixedPointPos)));
    }
    else
    {
        result = value * ((vx_float32) (1 << -fixedPointPos));
    }

    return result;

}
vx_int16 vnn_Fp32toInt16(vx_float32 val, vx_int8 fixedPointPos)
{
    vx_int16 result = 0;

    if (fixedPointPos > 0)
    {
        vx_int32 data = (vx_int32) round(val * (vx_float32)(1 << fixedPointPos));
        result = (vx_int16)((data > 32767) ? 32767 : (data < -32768) ? -32768 : data);

    }
    else
    {
        vx_int32 data = (vx_int32) round(val * (1.0f / (vx_float32)(1 << -fixedPointPos)));
        result = (vx_int16)((data > 32767) ? 32767 : (data < -32768) ? -32768 : data);
    }

    return result;
}
vx_int16 vnn_Fp32toAsymInt16(vx_float32 val, vx_int32 zeroPoint, vx_float32 scale)
{
    vx_int16 result = 0;
    vx_int32 data;

    data = (vx_int32) round((val / scale + (vx_int16)zeroPoint));

    if (data > 32767)
        data = 32767;

    if (data < -32768)
        data = -32768;

    result = (vx_int16)(data);

    return result;
}
vx_float32 vnn_AsymInt16toFp32(vx_int16 val, vx_int32 zeroPoint, vx_float32 scale)
{
    vx_float32 result = 0.0f;

    result = (val - (vx_int16)zeroPoint) * scale;
    return result;
}

vx_int16 vnn_Fp32toFp16(vx_float32 val)
{
#define F16_EXPONENT_BITS 0x1F
#define F16_EXPONENT_BIAS 15

#define F16_EXPONENT_SHIFT 10
#define F16_MANTISSA_BITS ((1 << F16_EXPONENT_SHIFT) - 1)
#define F16_MANTISSA_SHIFT (23 - F16_EXPONENT_SHIFT)
#define F16_MAX_EXPONENT (F16_EXPONENT_BITS << F16_EXPONENT_SHIFT)

    vx_uint32 f32 = (*(vx_uint32 *) &val);
    vx_int16 f16 = 0;
    /* Decode IEEE 754 little-endian 32-bit floating-point value */
    int sign = (f32 >> 16) & 0x8000;
    /* Map exponent to the range [-127,128] */
    int exponent = ((f32 >> 23) & 0xff) - 127;
    int mantissa = f32 & 0x007fffff;
    if (exponent == 128)
    { /* Infinity or NaN */
        if (mantissa)
        {
            /* Flush NaN to 0. */
            f16 = (vx_int16)sign;
        }
        else
        {
            /* Clamp to HALF_MAX/HALF_MIN. */
            f16 = (vx_int16)(sign | ((F16_EXPONENT_BITS - 1) << F16_EXPONENT_SHIFT) | F16_MANTISSA_BITS);
        }
    }
    else if (exponent > 15)
    { /* Overflow - clamp to HALF_MAX/HALF_MIN. */
        f16 = (vx_int16)(sign | ((F16_EXPONENT_BITS - 1) << F16_EXPONENT_SHIFT) | F16_MANTISSA_BITS);
    }
    else if (exponent > -15)
    { /* Representable value */
        /* RTNE */
        int roundingBit = (mantissa >> (F16_MANTISSA_SHIFT - 1)) & 0x1;
        int stickyBits = mantissa & 0xFFF;
        exponent += F16_EXPONENT_BIAS;
        mantissa >>= F16_MANTISSA_SHIFT;
        if (roundingBit)
        {
            if (stickyBits || (mantissa & 0x1))
            {
                mantissa++;
                if (mantissa > F16_MANTISSA_BITS)
                {
                    exponent++;
                    if (exponent > 30)
                    {
                        /* Clamp to HALF_MAX/HALF_MIN. */
                        exponent--;
                        mantissa--;
                    }
                    else
                    {
                        mantissa &= F16_MANTISSA_BITS;
                    }
                }
            }
        }
        f16 = (vx_int16)(sign | exponent << F16_EXPONENT_SHIFT | mantissa);
    }
    else
    {
        f16 = (vx_int16)sign;
    }
    return f16;
}

vx_float32 vnn_Fp16toFp32(const vx_uint16 in)
{
    vx_uint32 t1;
    vx_uint32 t2;
    vx_uint32 t3;
    vx_float32 out;

    t1 = in & 0x7fff;                       // Non-sign bits
    t2 = in & 0x8000;                       // Sign bit
    t3 = in & 0x7c00;                       // Exponent
    t1 <<= 13;                              // Align mantissa on MSB
    t2 <<= 16;                              // Shift sign bit into position
    t1 += 0x38000000;                       // Adjust bias
    t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero
    t1 |= t2;                               // Re-insert sign bit
    *((uint32_t*)&out) = t1;
    return out;
}

vx_uint32 vnn_GetTensorSize(vx_tensor tensor)
{
    vx_uint32 size[6];
    vx_uint32 num=1,num_of_dim=0;
    vx_uint32 i = 0;
    vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &num_of_dim, sizeof(num_of_dim));
    vxQueryTensor(tensor, VX_TENSOR_DIMS, size, sizeof(size));
    for(i = 0; i < num_of_dim;i++)
        num = num*size[i];
    return num;
}

vx_uint32 vnn_GetTensorDims(vx_tensor tensor)
{
    vx_uint32 size[6];
    vx_uint32 num_of_dim=0;
    vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &num_of_dim, sizeof(num_of_dim));
    return num_of_dim;
}

vx_uint32 vnn_GetTensorBufferSize(vx_tensor tensor)
{
    vx_uint32 size[6];
    vx_uint32 num=1,num_of_dim=0;
    vx_enum data_format;
    vx_uint32 i = 0;
    vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &num_of_dim, sizeof(num_of_dim));
    vxQueryTensor(tensor, VX_TENSOR_DIMS, size, sizeof(size));
    vxQueryTensor(tensor, VX_TENSOR_DATA_TYPE, &data_format, sizeof(data_format));
    num = vnn_GetTypeSize(data_format);
    for(i = 0; i < num_of_dim;i++)
        num = num*size[i];
    return num;
}

vx_status  vnn_CopyTensorToData(vx_tensor tensor,void **buf)
{
    vx_uint32 size[MAX_NUM_DIMS];
    vx_size   stride[MAX_NUM_DIMS];
	vx_size   start[MAX_NUM_DIMS];
	vx_size   end[MAX_NUM_DIMS];
    vx_uint32 num_of_dim=0;
    vx_status status = VX_SUCCESS;
    vx_uint32 i = 0;
    vx_enum data_format;
    vx_uint32 data_size = 0;
    vx_context context = vxGetContext((vx_reference)tensor);
    status |= vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &num_of_dim, sizeof(num_of_dim));
    status |=vxQueryTensor(tensor, VX_TENSOR_DIMS, size, sizeof(size));
    status |=vxQueryTensor(tensor, VX_TENSOR_DATA_TYPE, &data_format, sizeof(data_format));

    stride[0] = vnn_GetTypeSize(data_format);
	for (i=0; i< num_of_dim; i++)
    {
		start[i] = 0;
		end[i] = (vx_size)size[i];
		if(i>0)
		{
			stride[i] = stride[i - 1] * size[i - 1];
		}
    }
	data_size = vnn_GetTensorBufferSize(tensor);
    if(*buf == NULL)
        *buf = (void*)malloc(data_size);
	status = vxCopyTensorPatch(tensor, num_of_dim,start,end,stride,(void*)*buf, VX_READ_ONLY,0);

    return status;
}
vx_status  vnn_CopyTensorToFloat32Data(vx_tensor tensor,vx_float32 **buf)
{
    vx_uint32 size[6];
    vx_uint32 stride_size[6];
    vx_uint32 num=1,num_of_dim=0;
    vx_tensor_addressing user_addr = NULL;
    vx_status status = VX_SUCCESS;
    vx_uint32 i = 0;
    vx_enum data_format,quant_format;
    vx_uint32 data_size = 0;
    vx_context context = vxGetContext((vx_reference)tensor);
    void *data = NULL;

    status |=vxQueryTensor(tensor, VX_TENSOR_DATA_TYPE, &data_format, sizeof(data_format));
    status |=vxQueryTensor(tensor, VX_TENSOR_QUANT_FORMAT, &quant_format, sizeof(data_format));
    num = vnn_GetTensorSize(tensor);
    if(*buf == NULL)
        *buf = (vx_float32*)malloc(num * sizeof(vx_float32));
    status |= vnn_CopyTensorToData(tensor,&data);

    if(quant_format == VX_QUANT_DYNAMIC_FIXED_POINT && data_format == VX_TYPE_INT8)
    {
        vx_int8 fl=0;
        status |=vxQueryTensor(tensor, VX_TENSOR_FIXED_POINT_POSITION, &fl, sizeof(fl));
        if(data_format == VX_TYPE_INT8)
        {
            for(i = 0;i < num; i++)
                (*buf)[i]= vnn_Int8toFp32( ((vx_int8*)data)[i],fl);
        }
    }
    else if(quant_format == VX_QUANT_DYNAMIC_FIXED_POINT && data_format == VX_TYPE_INT16)
    {
        vx_int8 fl=0;
        status |=vxQueryTensor(tensor, VX_TENSOR_FIXED_POINT_POSITION, &fl, sizeof(fl));
        for(i = 0;i < num; i++)
             (*buf)[i]= vnn_Int16toFp32( ((vx_int16*)data)[i],fl);
    }
    else if(quant_format == VX_QUANT_AFFINE_SCALE &&  data_format == VX_TYPE_UINT8)
    {
        vx_int32 zp=0;
        vx_float32 scale;
        status |=vxQueryTensor(tensor, VX_TENSOR_ZERO_POINT, &zp, sizeof(zp));
        status |=vxQueryTensor(tensor, VX_TENSOR_SCALE, &scale, sizeof(scale));
        for(i = 0;i < num; i++)
            (*buf)[i]= vnn_Uint8toFp32( ((vx_uint8*)data)[i],zp,scale);
    }
	else if(quant_format == VX_QUANT_AFFINE_SCALE &&  data_format == VX_TYPE_INT8)
    {
        vx_int32 zp=0;
        vx_float32 scale;
        status |=vxQueryTensor(tensor, VX_TENSOR_ZERO_POINT, &zp, sizeof(zp));
        status |=vxQueryTensor(tensor, VX_TENSOR_SCALE, &scale, sizeof(scale));
        for(i = 0;i < num; i++)
            (*buf)[i]= vnn_AsymInt8toFp32( ((vx_int8*)data)[i],zp,scale);
    }
	else if(quant_format == VX_QUANT_AFFINE_SCALE &&  data_format == VX_TYPE_INT16)
    {
        vx_int32 zp=0;
        vx_float32 scale;
        status |=vxQueryTensor(tensor, VX_TENSOR_ZERO_POINT, &zp, sizeof(zp));
        status |=vxQueryTensor(tensor, VX_TENSOR_SCALE, &scale, sizeof(scale));
        for(i = 0;i < num; i++)
            (*buf)[i]= vnn_AsymInt16toFp32( ((vx_int8*)data)[i],zp,scale);
    }
	else if (quant_format ==VX_QUANT_NONE && data_format == VX_TYPE_UINT8)
	{
	    for(i = 0;i < num; i++)
             (*buf)[i] = ((vx_uint8*)data)[i];
	}
    else if(data_format == VX_TYPE_FLOAT16)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
             (*buf)[i]= vnn_Fp16toFp32( ((vx_int16*)data)[i]);
    }
    else if(data_format == VX_TYPE_FLOAT32)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
              (*buf)[i] = ((vx_float32*)data)[i];
    }
    else if(quant_format ==VX_QUANT_NONE && data_format == VX_TYPE_INT32)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
              (*buf)[i] = (vx_float32)(((vx_int32*)data)[i]);
    }
    else if(quant_format ==VX_QUANT_NONE && data_format == VX_TYPE_UINT32)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
              (*buf)[i] = (vx_float32)(((vx_uint32*)data)[i]);
    }
    else
    {
        printf("can't support format!\n");
        status = VX_FAILURE;
    }
    if(data){
        free(data);
    }
    return status;
}

vx_status  vnn_CopyDataToTensor(vx_tensor tensor,void *buf)
{
    vx_uint32 size[MAX_NUM_DIMS];
    vx_size   stride[MAX_NUM_DIMS];
	vx_size   start[MAX_NUM_DIMS];
	vx_size   end[MAX_NUM_DIMS];
    vx_uint32 num_of_dim=0;
    vx_tensor_addressing user_addr = NULL;
    vx_status status = VX_SUCCESS;
    vx_uint32 i = 0;
    vx_enum data_format;
    vx_uint32 data_size = 0;
    vx_context context = vxGetContext((vx_reference)tensor);
    status |= vxQueryTensor(tensor, VX_TENSOR_NUMBER_OF_DIMS, &num_of_dim, sizeof(num_of_dim));
    status |=vxQueryTensor(tensor, VX_TENSOR_DIMS, size, sizeof(size));
    status |=vxQueryTensor(tensor, VX_TENSOR_DATA_TYPE, &data_format, sizeof(data_format));

    stride[0] = vnn_GetTypeSize(data_format);
	for (i=0; i< num_of_dim; i++)
    {
		start[i] = 0;
		end[i] = (vx_size)size[i];
		if(i>0)
		{
			stride[i] = stride[i - 1] * size[i - 1];
		}
    }
	status = vxCopyTensorPatch(tensor, num_of_dim,start,end,stride,buf, VX_WRITE_ONLY,0);

    return status;
}
vx_status  vnn_CopyFloat32DataToTensor(vx_tensor tensor,vx_float32 *buf)
{
    vx_uint32 size[6];
    vx_uint32 stride_size[6];
    vx_uint32 num=1,num_of_dim=0;
    vx_tensor_addressing user_addr = NULL;
    vx_status status = VX_SUCCESS;
    vx_uint32 i = 0;
    vx_enum data_format,quant_format;
    vx_uint32 data_size = 0;
    vx_context context = vxGetContext((vx_reference)tensor);
    void *data = NULL;

    status |=vxQueryTensor(tensor, VX_TENSOR_DATA_TYPE, &data_format, sizeof(data_format));
    status |=vxQueryTensor(tensor, VX_TENSOR_QUANT_FORMAT, &quant_format, sizeof(data_format));
    num = vnn_GetTensorSize(tensor);
    data = (void*)malloc(vnn_GetTensorBufferSize(tensor));

    if(quant_format == VX_QUANT_DYNAMIC_FIXED_POINT && data_format == VX_TYPE_INT8)
    {
        vx_int8 fl=0;
        status |=vxQueryTensor(tensor, VX_TENSOR_FIXED_POINT_POSITION, &fl, sizeof(fl));
        if(data_format == VX_TYPE_INT8)
        {
            for(i = 0;i < num; i++)
                ((vx_int8*)data)[i] = vnn_Fp32toInt8(buf[i],fl);
        }
    }
    else if(quant_format == VX_QUANT_DYNAMIC_FIXED_POINT && data_format == VX_TYPE_INT16)
    {
        vx_int8 fl=0;
        status |=vxQueryTensor(tensor, VX_TENSOR_FIXED_POINT_POSITION, &fl, sizeof(fl));
        for(i = 0;i < num; i++)
             ((vx_int16*)data)[i] = vnn_Fp32toInt16(buf[i],fl);
    }
    else if(quant_format == VX_QUANT_AFFINE_SCALE &&  data_format == VX_TYPE_UINT8)
    {
        vx_int32 zp=0;
        vx_float32 scale;
        status |=vxQueryTensor(tensor, VX_TENSOR_ZERO_POINT, &zp, sizeof(zp));
        status |=vxQueryTensor(tensor, VX_TENSOR_SCALE, &scale, sizeof(scale));
        for(i = 0;i < num; i++)
            ((vx_uint8*)data)[i] = vnn_Fp32toUint8(buf[i],zp,scale);
    }
	else if(quant_format == VX_QUANT_AFFINE_SCALE &&  data_format == VX_TYPE_INT8)
    {
        vx_int32 zp=0;
        vx_float32 scale;
        status |=vxQueryTensor(tensor, VX_TENSOR_ZERO_POINT, &zp, sizeof(zp));
        status |=vxQueryTensor(tensor, VX_TENSOR_SCALE, &scale, sizeof(scale));
        for(i = 0;i < num; i++)
            ((vx_int8*)data)[i] = vnn_Fp32toAsymInt8(buf[i],zp,scale);
    }
	else if(quant_format == VX_QUANT_AFFINE_SCALE &&  data_format == VX_TYPE_INT16)
    {
        vx_int32 zp=0;
        vx_float32 scale;
        status |=vxQueryTensor(tensor, VX_TENSOR_ZERO_POINT, &zp, sizeof(zp));
        status |=vxQueryTensor(tensor, VX_TENSOR_SCALE, &scale, sizeof(scale));
        for(i = 0;i < num; i++)
            ((vx_int16*)data)[i] = vnn_Fp32toAsymInt16(buf[i],zp,scale);
    }
	else if(quant_format == VX_QUANT_NONE &&  data_format == VX_TYPE_UINT8)
    {
       for(i = 0;i < num; i++)
		   ((vx_uint8*)data)[i] = (vx_uint8)buf[i];
    }
    else if(data_format == VX_TYPE_FLOAT16)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
             ((vx_int16*)data)[i] = vnn_Fp32toFp16(buf[i]);
    }
    else if(data_format == VX_TYPE_FLOAT32)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
              ((vx_float32*)data)[i] = buf[i];
    }
    else if(quant_format == VX_QUANT_NONE &&  data_format == VX_TYPE_INT32)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
              ((vx_int32*)data)[i] = (vx_int32)buf[i];
    }
    else if(quant_format == VX_QUANT_NONE &&  data_format == VX_TYPE_UINT32)/*VX_QUANT_NONE*/
    {
         for(i = 0;i < num; i++)
              ((vx_uint32*)data)[i] = (vx_uint32)buf[i];
    }
    else
    {
        printf("can't support format!\n");
        status = VX_FAILURE;
    }
    status |= vnn_CopyDataToTensor(tensor,data);
    if(data){
        free(data);
    }
    return status;
}


static file_type get_file_type(const char *file_name)
{
    const char *ptr;
    char sep = '.';
    uint32_t pos,n;
    char buff[32] = {0};

    ptr = strrchr(file_name, sep);
    pos = ptr - file_name;
    n = strlen(file_name) - (pos + 1);
    strncpy(buff, file_name+(pos+1), n);

    if(strcmp(buff, "tensor") == 0
        || strcmp(buff, "txt") == 0)
    {
		return FILE_TYPE_TEXT;
    }
    else if(strcmp(buff, "bin") == 0
        || strcmp(buff, "dat") == 0)
    {
        return FILE_TYPE_BIN;
    }
    else
    {
       return FILE_TYPE_NOT_SUPPORT;
    }
}

vx_status vnn_LoadTensorRandom(vx_tensor tensor)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
    char dumpInput[128];

    vx_int32 num = vnn_GetTensorSize(tensor);
    vx_uint8 *buf = (vx_uint8*)malloc(num*sizeof(vx_uint8));
    memset(buf, 0, num * sizeof(vx_uint8));

    status |= vnn_CopyDataToTensor(tensor,buf);
    _CHECK_STATUS(status, exit);
    //printf("Info: Copied a buffer of a size of %d to tensor.\n", num);
    snprintf(dumpInput, sizeof(dumpInput), "input.dat");
    vnn_SaveTensorToFileAsBinary(tensor, dumpInput);
    status = VX_SUCCESS;
    if(buf)
        free(buf);
exit:
	return status;
}

vx_status vnn_LoadTensorFromFile(vx_tensor tensor,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
    file_type ft = get_file_type(filename);

	if(ft == FILE_TYPE_TEXT) /*tensor*/
    {
        vx_int32 num = vnn_GetTensorSize(tensor);
        vx_float32 *buf = (vx_float32*)malloc(num*sizeof(vx_float32));
        FILE *fp = fopen(filename,"r");
        if(fp)
        {
            for(i = 0;i < num;i++)
            {
                vx_int32 ret = fscanf(fp,"%f",&buf[i]);
                if(ret <=0 )
                {
                    printf("There is no enough data!\n");
                    status = VX_FAILURE;
                    break;
                }
            }
            status |= vnn_CopyFloat32DataToTensor(tensor,buf);
        }
        else
        {
             printf("can't open file %s\n",filename);
             status |= VX_FAILURE;
        }

        if(buf){
            free(buf);
        }
        if(fp)
            fclose(fp);
    }
	else if(ft == FILE_TYPE_BIN) /*dat or bin*/
    {
        vx_int32 size = vnn_GetTensorBufferSize(tensor);
        void *buf = (void*)malloc(size);
        FILE  *fp = fopen(filename,"rb");
        if(fp)
        {
            vx_int32 ret = fread(buf,1,size,fp);
            if(ret < size)
            {
                printf("There is no enough data!\n");
                status = VX_FAILURE;
            }
			vnn_CopyDataToTensor(tensor,buf);
        }
        else
        {
             printf("can't open file %s\n",filename);
             status |= VX_FAILURE;
        }
        if(buf){
            free(buf);
        }
        if(fp)
            fclose(fp);
    }
    else
    {
        printf("not support file type:%s\n",filename);
        status |= VX_FAILURE;
    }
    return status;
}
vx_status  vnn_SaveTensorToFileAsFloat32(vx_tensor tensor,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_float32 *buf = NULL;
    vx_int32 i=0;
    FILE *fp = NULL;
    vx_uint32 size[6];
    vx_uint32 num=0;
	num = vnn_GetTensorSize(tensor);
	fp = fopen(filename,"w");
    if(fp)
    {
        status |= vnn_CopyTensorToFloat32Data(tensor,&buf);
        for(i = 0;i<num;i++)
            fprintf(fp,"%f\n",buf[i]);
        fclose(fp);
    }
    else
        status = VX_FAILURE;
    if(buf){
        free(buf);
    }
    return status;
}
vx_status  vnn_SaveTensorToFileAsBinary(vx_tensor tensor,char *filename)
{
    vx_status status = VX_SUCCESS;
    void *buf = NULL;
    vx_int32 i=0;
    FILE *fp = NULL;
    vx_uint32 size[6];
    vx_uint32 num_of_dim=0;
    vx_uint32 buf_size = vnn_GetTensorBufferSize(tensor);
    fp = fopen(filename,"wb");
    if(fp)
    {
        status |= vnn_CopyTensorToData(tensor,&buf);
        fwrite(buf,1,buf_size,fp);
        fclose(fp);
    }
    else
        status = VX_FAILURE;

    if(buf){
        free(buf);
    }
    return status;
}



static vx_status LoadBinaryFromFile(char *filename,void *buf, vx_int32 size)
{
	FILE *fp = fopen(filename,"rb");
	vx_int32 ret = fread(buf,1,size,fp);
	fclose(fp);
	return ret == size ?  VX_SUCCESS :  VX_FAILURE;
}
static vx_status LoadTextFromFile(char *filename,void *buf, vx_int32 size,vx_enum data_type)
{
	vx_status status = VX_SUCCESS;
	vx_float32 *temp = (vx_float32*)malloc(size*sizeof(vx_float32));
	FILE *fp = fopen(filename,"r");
	vx_int32 elm_size  = vnn_GetTypeSize(data_type);
	vx_int32 count = size/elm_size;
	for(int i=0;i< count;i++)
		fscanf(fp,"%f",&temp[i]);
	fclose(fp);
	switch(data_type)
    {
        case VX_TYPE_INT8:
			for(int i=0;i< count;i++)
				((vx_int8*)buf)[i] = (vx_int8)temp[i];
			break;
        case VX_TYPE_UINT8:
		case VX_DF_IMAGE_U8:
			for(int i=0;i< count;i++)
				((vx_uint8*)buf)[i] = (vx_uint8)temp[i];
            break;
        case VX_TYPE_INT16:
		case VX_DF_IMAGE_S16:
			for(int i=0;i< count;i++)
				((vx_int16*)buf)[i] = (vx_int16)temp[i];
			break;
		case VX_DF_IMAGE_U16:
		case VX_TYPE_UINT16:
			for(int i=0;i< count;i++)
				((vx_uint16*)buf)[i] = (vx_uint16)temp[i];
            break;
        case VX_TYPE_INT32:
		case VX_DF_IMAGE_S32:
			for(int i=0;i< count;i++)
				((vx_int32*)buf)[i] = (vx_int32)temp[i];
			break;
        case VX_TYPE_UINT32:
		case VX_DF_IMAGE_U32:
			for(int i=0;i< count;i++)
				((vx_uint32*)buf)[i] = (vx_uint32)temp[i];
			break;
        case VX_TYPE_FLOAT32:
		case VX_DF_IMAGE_F32:
            for(int i=0;i< count;i++)
				((vx_float32*)buf)[i] = (vx_float32)temp[i];
			break;

		default:
			printf("Not support format:%d,line=%d\n",data_type,__LINE__);
			status = VX_FAILURE;
    }
	if(temp){
		free(temp);
    }
	return status;
}
static vx_status SaveBinaryToFile(char *filename,void *buf, vx_int32 size)
{
	FILE *fp = fopen(filename,"wb");
	vx_int32 ret = fwrite(buf,1,size,fp);
	fclose(fp);
	return ret == size ?  VX_SUCCESS :  VX_FAILURE;
}
static vx_status SaveTextToFile(char *filename,void *buf, vx_int32 size,vx_enum data_type)
{
	vx_status status = VX_SUCCESS;
	vx_int32 elm_size  = vnn_GetTypeSize(data_type);
	vx_int32 count = size/elm_size;
	vx_float32 *temp = (vx_float32*)malloc(count*sizeof(vx_float32));

	switch(data_type)
    {
        case VX_TYPE_INT8:
			for(int i=0;i< count;i++)
				temp[i] = (vx_float32)((vx_int8*)buf)[i];
			break;
        case VX_TYPE_UINT8:
		case VX_DF_IMAGE_U8:
			for(int i=0;i< count;i++)
				temp[i] = (vx_float32)((vx_uint8*)buf)[i];
            break;
        case VX_TYPE_INT16:
		case VX_DF_IMAGE_S16:
			for(int i=0;i< count;i++)
				temp[i] = (vx_float32)((vx_int16*)buf)[i];
			break;
		case VX_DF_IMAGE_U16:
		case VX_TYPE_UINT16:
			for(int i=0;i< count;i++)
				temp[i] = (vx_float32)((vx_uint16*)buf)[i];
            break;
        case VX_TYPE_INT32:
		case VX_DF_IMAGE_S32:
			for(int i=0;i< count;i++)
				temp[i] = (vx_float32)((vx_int32*)buf)[i];
			break;
        case VX_TYPE_UINT32:
		case VX_DF_IMAGE_U32:
			for(int i=0;i< count;i++)
				temp[i] = (vx_float32)((vx_uint32*)buf)[i];
			break;
        case VX_TYPE_FLOAT32:
		case VX_DF_IMAGE_F32:
            for(int i=0;i< count;i++)
				temp[i] = ((vx_float32*)buf)[i];
			break;
		default:
			printf("Not support format:%d,line=%d\n",data_type,__LINE__);
			status = VX_FAILURE;
    }
	FILE *fp = fopen(filename,"w");
	for(int i=0;i< count;i++)
		fprintf(fp,"%f\n",temp[i]);
	fclose(fp);
	if(temp){
		free(temp);
    }
	return status;
}

vx_status vnn_LoadImageFromFile(vx_image image,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
	file_type ft = get_file_type(filename);

	vx_imagepatch_addressing_t imgInfo = VX_IMAGEPATCH_ADDR_INIT;
	vx_uint32 width,height;
	vx_df_image format;
	vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
	vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
	vxQueryImage(image, VX_IMAGE_FORMAT, &format, sizeof(format));
	void* ptr = NULL;
	vx_rectangle_t rect = {0,0,width,height};
	vx_map_id map_id = 0;
	vx_int32 size =  width*height*vnn_GetTypeSize(format);
	status |= vxMapImagePatch(image, &rect, 0, &map_id, &imgInfo, (void**)&ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);
	if(ft == FILE_TYPE_TEXT)
		LoadTextFromFile(filename,ptr,size,format);
	else if(ft == FILE_TYPE_BIN)
		LoadBinaryFromFile(filename,ptr,size);
	else
	{
		status = VX_FAILURE;
		printf("Not support file type:%d,line=%d\n",filename,__LINE__);
	}
	status |= vxUnmapImagePatch(image, map_id);
    return status;
}
vx_status vnn_SaveImageToFile(vx_image image,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
	file_type ft = get_file_type(filename);

	vx_imagepatch_addressing_t imgInfo = VX_IMAGEPATCH_ADDR_INIT;
	vx_uint32 width,height;
	vx_df_image format;
	vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
	vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
	vxQueryImage(image, VX_IMAGE_FORMAT, &format, sizeof(format));
	void* ptr = NULL;
	vx_rectangle_t rect = {0,0,width,height};
	vx_map_id map_id = 0;
	vx_int32 size =  width*height*vnn_GetTypeSize(format);
	status |= vxMapImagePatch(image, &rect, 0, &map_id, &imgInfo, (void**)&ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
	if(ft == FILE_TYPE_TEXT)
		SaveTextToFile(filename,ptr,size,format);
	else if(ft == FILE_TYPE_BIN)
		SaveBinaryToFile(filename,ptr,size);
	else
	{
		status = VX_FAILURE;
		printf("Not support file type:%d,line=%d\n",filename,__LINE__);
	}
	status |= vxUnmapImagePatch(image, map_id);
    return status;
}
vx_status vnn_LoadArrayFromFile(vx_array arr,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
    file_type ft = get_file_type(filename);
	vx_size itemNumber = 0;
	vx_size itemSize = 0;
	vx_enum itemType;
	vxQueryArray(arr, VX_ARRAY_CAPACITY, &itemNumber, sizeof(itemNumber));
    vxQueryArray(arr, VX_ARRAY_ITEMSIZE, &itemSize, sizeof(itemSize));
	vxQueryArray(arr, VX_ARRAY_ITEMTYPE, &itemType, sizeof(itemType));
	void* ptr = NULL;
	vx_map_id map_id = 0;
	vx_int32 size = itemNumber*itemSize;
	status |= vxMapArrayRange(arr, 0, itemNumber,&map_id, &itemSize, (void**)&ptr, VX_WRITE_ONLY,VX_MEMORY_TYPE_HOST,0);
	if(ft == FILE_TYPE_TEXT)
		LoadTextFromFile(filename,ptr,size,itemType);
	else if(ft == FILE_TYPE_BIN)
		LoadBinaryFromFile(filename,ptr,size);
	else
	{
		status = VX_FAILURE;
		printf("Not support file type:%d,line=%d\n",filename,__LINE__);
	}
	status |= vxUnmapArrayRange(arr, map_id);
    return status;
}
vx_status vnn_SaveArrayToFile(vx_array arr,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
    file_type ft = get_file_type(filename);
	vx_size itemNumber = 0;
	vx_size itemSize = 0;
	vx_enum itemType;
	vxQueryArray(arr, VX_ARRAY_CAPACITY, &itemNumber, sizeof(itemNumber));
    vxQueryArray(arr, VX_ARRAY_ITEMSIZE, &itemSize, sizeof(itemSize));
	vxQueryArray(arr, VX_ARRAY_ITEMTYPE, &itemType, sizeof(itemType));
	void* ptr = NULL;
	vx_map_id map_id = 0;
	vx_int32 size = itemNumber*itemSize;
	status |= vxMapArrayRange(arr, 0, itemNumber,&map_id, &itemSize, (void**)&ptr, VX_READ_ONLY,VX_MEMORY_TYPE_HOST,0);
	if(ft == FILE_TYPE_TEXT)
		SaveTextToFile(filename,ptr,size,itemType);
	else if(ft == FILE_TYPE_BIN)
		SaveBinaryToFile(filename,ptr,size);
	else
	{
		status = VX_FAILURE;
		printf("Not support file type:%d,line=%d\n",filename,__LINE__);
	}
	status |= vxUnmapArrayRange(arr, map_id);
    return status;
}
vx_status vnn_LoadScalarFromFile(vx_scalar scalar,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
    file_type ft = get_file_type(filename);
	vx_enum data_type;
	vx_float32 value;
	vxQueryScalar(scalar, VX_SCALAR_TYPE, &data_type, sizeof(data_type));
	vx_int32 size = vnn_GetTypeSize(data_type);
	if(ft == FILE_TYPE_TEXT)
		LoadTextFromFile(filename,&value,size,data_type);
	else if(ft == FILE_TYPE_BIN)
		LoadBinaryFromFile(filename,&value,size);
	else
	{
		status = VX_FAILURE;
		printf("Not support file type:%d,line=%d\n",filename,__LINE__);
	}
	vxCopyScalar(scalar,&value,VX_WRITE_ONLY,VX_MEMORY_TYPE_HOST);
    return status;
}
vx_status vnn_SaveScalarToFile(vx_scalar scalar,char *filename)
{
    vx_status status = VX_SUCCESS;
    vx_int32 i=0;
    file_type ft = get_file_type(filename);
	vx_enum data_type;
	vx_float32 value;
	vxQueryScalar(scalar, VX_SCALAR_TYPE, &data_type, sizeof(data_type));
	vx_int32 size = vnn_GetTypeSize(data_type);
	vxCopyScalar(scalar,&value,VX_READ_ONLY,VX_MEMORY_TYPE_HOST);
	if(ft == FILE_TYPE_TEXT)
		SaveTextToFile(filename,&value,size,data_type);
	else if(ft == FILE_TYPE_BIN)
		SaveBinaryToFile(filename,&value,size);
	else
	{
		status = VX_FAILURE;
		printf("Not support file type:%d,line=%d\n",filename,__LINE__);
	}
    return status;
}
vx_status vnn_LoadDataFromFile(inout_obj* obj,char *filename)
{
	vx_status status = VX_FAILURE;
	if(obj->data_type == VX_TYPE_TENSOR)
    {
        status = vnn_LoadTensorFromFile(obj->u.tensor,filename);
        _CHECK_STATUS(status, exit);
    }
	else if(obj->data_type == VX_TYPE_IMAGE)
    {
        status = vnn_LoadImageFromFile(obj->u.image,filename);
        _CHECK_STATUS(status, exit);
    }
	else if(obj->data_type == VX_TYPE_ARRAY)
    {
		status = vnn_LoadArrayFromFile(obj->u.array,filename);
        _CHECK_STATUS(status, exit);
    }
	else if(obj->data_type == VX_TYPE_SCALAR)
    {
		status = vnn_LoadScalarFromFile(obj->u.scalar,filename);
        _CHECK_STATUS(status, exit);
    }
    else
    {
        status = VX_FAILURE;
        _CHECK_STATUS(status, exit);
    }
exit:
	return status;
}
vx_status vnn_SaveDataToFile(inout_obj* obj,char *filename)
{
	vx_status status = VX_FAILURE;
	if(obj->data_type == VX_TYPE_TENSOR)
    {
		file_type  ft = get_file_type(filename);
		if(ft == FILE_TYPE_TEXT)
			status = vnn_SaveTensorToFileAsFloat32(obj->u.tensor,filename);
		else if (ft == FILE_TYPE_BIN)
			status = vnn_SaveTensorToFileAsBinary(obj->u.tensor,filename);
		else
			status = VX_FAILURE;
        _CHECK_STATUS(status, exit);
    }
	else if(obj->data_type == VX_TYPE_IMAGE)
    {
        status = vnn_SaveImageToFile(obj->u.image,filename);
        _CHECK_STATUS(status, exit);
    }
	else if(obj->data_type == VX_TYPE_ARRAY)
    {
		status = vnn_SaveArrayToFile(obj->u.array,filename);
        _CHECK_STATUS(status, exit);
    }
	else if(obj->data_type == VX_TYPE_SCALAR)
    {
		status = vnn_SaveScalarToFile(obj->u.scalar,filename);
        _CHECK_STATUS(status, exit);
    }
    else
    {
        status = VX_FAILURE;
        _CHECK_STATUS(status, exit);
    }
exit:
	return status;
}

vx_bool get_top
    (
    float *pfProb,
    float *pfMaxProb,
    uint32_t *pMaxClass,
    uint32_t outputCount,
    uint32_t topNum
    )
{
    uint32_t i, j;

    #define MAX_TOP_NUM 20
    if (topNum > MAX_TOP_NUM) return vx_false_e;

    memset(pfMaxProb, 0, sizeof(float) * topNum);
    memset(pMaxClass, 0xff, sizeof(float) * topNum);

    for (j = 0; j < topNum; j++)
    {
        for (i=0; i<outputCount; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (pfProb[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = pfProb[i];
                *(pMaxClass+j) = i;
            }
        }
    }

    return vx_true_e;
}
vx_status vnn_ShowTensorTop5(vx_tensor tensor)
{
    vx_status status = VX_SUCCESS;
    vx_float32 *buf = NULL;
    vx_int32 num = vnn_GetTensorSize(tensor);
    uint32_t MaxClass[5];
    vx_float32 fMaxProb[5];
    vx_int32 i=0;
    status |= vnn_CopyTensorToFloat32Data(tensor,&buf);
    if (!get_top(buf, fMaxProb, MaxClass, num, 5))
    {
        printf("Fail to show result.\n");
        status |= VX_SUCCESS;
        goto final;
    }

    printf(" --- Top5 ---\n");
    for(i=0; i<5; i++)
    {
        printf("%3d: %8.6f\n", MaxClass[i], fMaxProb[i]);
    }
final:
    if(buf){
        free(buf);
    }
    return status;
}
vx_status  vnn_ShowTop5(inout_obj* obj)
{
	if(obj->data_type == VX_TYPE_TENSOR)
		return vnn_ShowTensorTop5(obj->u.tensor);

	return VX_SUCCESS;
}
vx_status vnn_QueryInputsAndOutputsParam(vx_kernel kernel,inout_param *input,vx_int32 *in_cnt,inout_param *output,vx_int32 *out_cnt)
{
	vx_status status = VX_SUCCESS;
	int i = 0;
	/* Query number of parameters in kernel. num_params = input_count + output_count */
	vx_uint32 num_params;
	vx_int32 input_count=0,output_count=0;
	vx_enum direction = 0;
	vxQueryKernel(kernel, VX_KERNEL_PARAMETERS, &num_params, sizeof(vx_uint32));

	for (i = 0; i < num_params; i++)
	{
		inout_param *ptrInOut = NULL;
		vx_parameter param_kernel = NULL;
		vx_meta_format meta;
		vx_size  _dim_size[MAX_NUM_DIMS],_dim_count;
		param_kernel = vxGetKernelParameterByIndex(kernel, i);
		vxQueryParameter(param_kernel, VX_PARAMETER_DIRECTION, &direction, sizeof(enum vx_type_e));
		if(direction == VX_INPUT)
		{
			ptrInOut = &input[input_count];
			input_count++;
		}
		else
		{
			ptrInOut = &output[output_count];
			output_count++;
		}
		vxQueryParameter(param_kernel,VX_PARAMETER_META_FORMAT,&meta, sizeof(vx_meta_format));
		vxQueryMetaFormatAttribute(meta, VX_REFERENCE_TYPE,	&ptrInOut->data_type, sizeof(ptrInOut->data_type));
        vxQueryParameter(param_kernel,VX_PARAMETER_META_FORMAT,  &meta, sizeof(vx_meta_format));
		vxQueryMetaFormatAttribute(meta, VX_TENSOR_NUMBER_OF_DIMS,&_dim_count, sizeof(_dim_count));
		ptrInOut->dim_count = _dim_count;
		vxQueryMetaFormatAttribute(meta, VX_TENSOR_DIMS,	&_dim_size, sizeof(_dim_size));
		for(int n = 0;n<ptrInOut->dim_count;n++)
			ptrInOut->dim_size[n] = _dim_size[n];
		vxQueryMetaFormatAttribute(meta, VX_TENSOR_DATA_TYPE, &ptrInOut->data_format, sizeof(ptrInOut->data_format));
		vxQueryMetaFormatAttribute(meta, VX_TENSOR_QUANT_FORMAT, &ptrInOut->quan_format, sizeof(ptrInOut->quan_format));
		switch (ptrInOut->quan_format)
		{
			case VX_QUANT_DYNAMIC_FIXED_POINT:
				vxQueryMetaFormatAttribute(meta,VX_TENSOR_FIXED_POINT_POSITION,	&ptrInOut->fixed_pos, sizeof(ptrInOut->fixed_pos));
				break;
			case VX_QUANT_AFFINE_SCALE:
				vxQueryMetaFormatAttribute(meta, VX_TENSOR_ZERO_POINT, &ptrInOut->tf_zerop, sizeof(ptrInOut->tf_zerop));
				vxQueryMetaFormatAttribute(meta, VX_TENSOR_SCALE, &ptrInOut->tf_scale, sizeof(ptrInOut->tf_scale));
				break;
			case VX_QUANT_NONE:
				break;
			case VX_QUANT_AFFINE_SCALE_PER_CHANNEL:
			default:
				status = VX_FAILURE;
				printf("Quant format %u is not supported!", ptrInOut->quan_format);
				_CHECK_STATUS(status, exit);
				break;
		}
		vnn_Log("index:%d,in:%d, data_type:%d\ndim_cnt: %d, dims: %d, %d, %d, %d\nscale: %f, zp: %d, fixPos: %d\nquant_format:%d,data_format: %d\n", i,direction,  ptrInOut->data_type,
			ptrInOut->dim_count, ptrInOut->dim_size[0], ptrInOut->dim_size[1],ptrInOut->dim_size[2], ptrInOut->dim_size[3],
			ptrInOut->tf_scale, ptrInOut->tf_zerop,	ptrInOut->fixed_pos, ptrInOut->quan_format,
			ptrInOut->data_format);
	}
	*in_cnt	= input_count;
	*out_cnt = output_count;
exit:
	return status;

}

vx_status  vnn_CreateObject(vx_context context, inout_param *param, inout_obj *obj)
{
	vx_status status = VX_SUCCESS;
	if(param->data_type == VX_TYPE_TENSOR)
	{
		vx_tensor_create_params_t ts_params;
		ts_params.data_format                 = param->data_format;
		ts_params.num_of_dims                 = param->dim_count;
		ts_params.sizes                       = param->dim_size;
		ts_params.quant_format                = param->quan_format;
		if(param->quan_format == VX_QUANT_AFFINE_SCALE)
		{
			ts_params.quant_data.affine.scale     = param->tf_scale;
			ts_params.quant_data.affine.zeroPoint = param->tf_zerop;
		}
		else if(param->quan_format == VX_QUANT_DYNAMIC_FIXED_POINT)
		{
			ts_params.quant_data.dfp.fixed_point_pos = (vx_int8)param->fixed_pos;
		}
		else
		{
			/*nothing*/
		}
		obj->u.tensor = vxCreateTensor2(context, &ts_params, sizeof(ts_params));
		obj->data_type = param->data_type;
		_CHECK_OBJ(obj->u.tensor,exit);
	}
	else if (param->data_type == VX_TYPE_IMAGE)
	{
		obj->u.image = vxCreateImage(context, param->dim_size[0],param->dim_size[1],param->data_format);
		obj->data_type = VX_TYPE_IMAGE;
		_CHECK_OBJ(obj->u.image,exit);
	}
	else if (param->data_type == VX_TYPE_ARRAY)
	{
		obj->u.array = vxCreateArray(context, param->data_format, param->dim_size[0]);
		obj->data_type = VX_TYPE_ARRAY;
		_CHECK_OBJ(obj->u.array,exit);
		for(int i=0; i <  param->dim_size[0];i++)
		{
			vx_float32 zero = 0;
			vxAddArrayItems(obj->u.array,1,&zero,0);
		}
	}
	else if (param->data_type == VX_TYPE_SCALAR)
	{
		vx_float32 num = 0;
		obj->u.scalar =  vxCreateScalar(context, param->data_format, &num);
		obj->data_type = VX_TYPE_SCALAR;
		_CHECK_OBJ(obj->u.scalar,exit);
	}
	else
	{
		printf("Not support output data type:%d\n",param->data_type);
		status = VX_FAILURE;
	}
exit:
	return status;
}
vx_status  vnn_ReleaseObject(inout_obj *obj)
{
	vx_status status = VX_SUCCESS;
	if(obj->data_type == VX_TYPE_TENSOR)
	{
		vxReleaseTensor( &obj->u.tensor);
	}
	else if (obj->data_type == VX_TYPE_IMAGE)
	{
		vxReleaseImage( &obj->u.image);
	}
	else if (obj->data_type == VX_TYPE_ARRAY)
	{
		vxReleaseArray( &obj->u.array);
	}
	else if (obj->data_type == VX_TYPE_SCALAR)
	{
		vxReleaseScalar( &obj->u.scalar);
	}
	else
	{
		printf("Not support output data type:%d\n",obj->data_type);
		status = VX_FAILURE;
	}
	return status;
}

void vnn_Log
    (
    const char *fmt,
    ...
    )
{
    char arg_buffer[VSI_NN_MAX_DEBUG_BUFFER_LEN] = {0};
    va_list arg;
	vx_int32 debug = 0;

	char *debug_s = getenv("VNN_DEBUG");
	if(debug_s)
    {
        debug = atoi(debug_s);
    }

    if(debug != 1)
    {
        return ;
    }
    va_start(arg, fmt);
    vsnprintf(arg_buffer, VSI_NN_MAX_DEBUG_BUFFER_LEN, fmt, arg);
    va_end(arg);
    fprintf(stderr, "%s\n", arg_buffer);
}
