#ifndef __FFT_H__
#define __FFT_H__

#include "main.h"
#include "stdio.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "arm_common_tables.h"
#include "dsp/complex_math_functions.h"

#define FFT_LENGTH 256

extern float FFT_inputbuf[FFT_LENGTH*2];
extern float FFT_outputbuf[FFT_LENGTH];

void FFT_inputbuff_init(float FFT_inputbuf[],float ADC_Value[],uint32_t length);

void IFFT_inputbuff_init(float IFFT_inputbuf[],float need_IFFT_value[],uint32_t length);

void FFT_Start(float ADC_Value[],float FFT_inputbuf[],float FFT_outputbuf[],uint32_t length);

void IFFT_Start(float need_IFFT_value[],float IFFT_inputbuf[],float IFFT_outputbuf[],uint32_t length);

void FFT_printf_result(float printf_buf[],uint32_t length);

//生成汉宁窗
void hanning_window(float *w,uint32_t length);
 
//计算序列的自卷积
void convolve(float *w, float *y,uint32_t length);

// 计算序列的和
float sum(float *x, int n);

// 归一化序列
void normalize(float *x, int n);

//相位计算函数
float Phase_atan(float *inputSignal,uint32_t index);

// void ifft()



#endif
