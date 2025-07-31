//使用V6才能调用DSP库，但8688的时序无法满足,怀疑是编译器把变量优化了，但使用了volatile也没用
//使用V5不能使用DSP库(keil添加)，但8688时序满足，怀疑是DSP库版本兼容性问题
//最终选择使用version5编译器+CubeMX添加DSP库(CubeMX中的DSP有部分函数不能用，且版本较老1.3)

//FFT的准确性取决于对波形的还原程度，与采样周期数关系不大
//相同采样点下，采样的周期数越多，波形还原程度越差，FFT越不准确


#include "FFT.h"

// float AD8688_value[FFT_LENGTH];//8688采集数据数组
float FFT_inputbuf[FFT_LENGTH*2];
float FFT_outputbuf[FFT_LENGTH];

//arm_max_f32(value,150,&AD8688_Max,&AD8688_Max_num);//找最大值函数,参数1：数组，参数2：数组大小，参数3：最大值变量地址，参数4：最大值索引地址，参数3,4必须为32位

//arm_mean_f32(value,150,&AD8688_Mean);//找平均值函数

/***
 * 
 */
/**
 * @brief 对输入数组初始化，实部为ADC采集值，虚部全为0
 * @param FFT输入数组
 * @param ADC采集电压数组
 * @param FFT运算个数
*/
void FFT_inputbuff_init(float FFT_inputbuf[],float ADC_Value[],uint32_t length)
{
 //FFT输入数组初始化
	for (int i = 0; i < length; i++)
	{
		FFT_inputbuf[i * 2] = ADC_Value[i];//实部赋值，实部为ADC实际采集电压
		FFT_inputbuf[i * 2 + 1] = 0;//虚部赋值，固定为0.
	}
}

/**
 * @brief 进行FFT运算，将ADC采集电压值存入FFT输入数组并进行实部虚部处理，再进行FFT运算并处理，结果存入FFT输出数组
 * @param ADC采集电压数组
 * @param FFT输入数组
 * @param FFT输出数组
 * @param FFT运算个数
*/
void FFT_Start(float ADC_Value[],float FFT_inputbuf[],float FFT_outputbuf[],uint32_t length)
{
    FFT_inputbuff_init(FFT_inputbuf,ADC_Value,length);//对输入数组进行FFT初始化

    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_inputbuf, 0, 1);//对输入数据进行傅里叶变换，arm_cfft_sR_f32_len256：傅里叶变换结构体，256是要运算的点数，根据实际修改
	arm_cmplx_mag_f32(FFT_inputbuf, FFT_outputbuf, length); //对输入数组取模

    //对运算结果再处理，第一个数据除以FFT_LENGTH，其余数据除以FFT_LENGTH的一半（与FFT算法有关）
	FFT_outputbuf[0] /= length;
	for (int i = 1; i < length; i++)//输出各次谐波幅值
	{
		FFT_outputbuf[i] /= (length/2);
	}
}

/**
 * @brief 将数组结果打印,以两位小数的形式打印
 * @param 需要打印的数组
 * @param 数组长度
*/
void FFT_printf_result(float printf_buf[],uint32_t length)
{
    printf("FFT Result:\r\n");
	for (int i = 0; i < length; i++)//输出各次谐波幅值
	{
		printf("%d\t%.2f\n",i,printf_buf[i]);
	}
}

/**********全相位测量*************************************************************************************************************************************************/

//生成汉宁窗
void hanning_window(float *w,uint32_t length)
{
    uint16_t n;
    for (n = 0; n < length; n++)
    {
        w[n] = 0.5 * (1 - cos(2 * PI * n / length));
    }
}
 
//计算序列的自卷积
void convolve(float *w, float *y,uint32_t length)
{
    int n, m;
    for (n = 0; n < 2 * length - 1; n++) {
        y[n] = 0;
        for (m = 0; m < length; m++) {
            if (n - m >= 0 && n - m < length) {
                y[n] += w[m] * w[n - m];
            }
        }
    }
}
 
// 计算序列的和
float sum(float *x, int n)
{
    int i;
    float s = 0;
    for (i = 0; i < n; i++) {
        s += x[i];
    }
    return s;
}
 
// 归一化序列
void normalize(float *x, int n)
{
    float s = sum(x, n);
    int i;
    for (i = 0; i < n; i++) {
        x[i] /= s;
    }
}

//相位计算函数
float Phase_atan(float *inputSignal,uint32_t index)
{
    if(inputSignal[2*index+1] >= 0 && inputSignal[2*index] >= 0)
        return   0 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    else if(inputSignal[2*index+1] >= 0 && inputSignal[2*index] <= 0)
        return 180 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    else if(inputSignal[2*index+1] <= 0 && inputSignal[2*index] <= 0)
        return 180 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    else if(inputSignal[2*index+1] <= 0 && inputSignal[2*index] >= 0)
        return 360 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
}


/**
void AP_FFT(void)
{
    for (int i = 0; i < 2*FFT_LENGTH-1; i++)
    {
        wave_2[i]=AD8688_value[i];
        wave_1[i]=AD8688_value[i];
    }
//    Printf(wave_1,Length);
 
    for(int i = 0; i < 2*FFT_LENGTH-1; i++)
    {
        wave_1[i] = wave_1[i] * fft_conv[i];    //加窗
        wave_2[i] = wave_2[i] * fft_conv[i];    //加窗
    }
//   Printf(fft_conv,Length*2-1);
    for(int i = 0; i < FFT_LENGTH-1; i++)
    {
        wave_1[i] = wave_1[i] + wave_1[FFT_LENGTH+i];
        wave_2[i] = wave_2[i] + wave_2[FFT_LENGTH+i];
    }
//    Printf(wave_1,Length);
    for (int i = 0; i < FFT_LENGTH; i++)
    {
        FFT_inputbuf_1[i * 2] = wave_1[i];
        FFT_inputbuf_1[i * 2 + 1] = 0;
        FFT_inputbuf_2[i * 2] = wave_2[i];
        FFT_inputbuf_2[i * 2 + 1] = 0;
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_inputbuf_1, 0, 1);
    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_inputbuf_2, 0, 1);
    
 
    arm_cmplx_mag_f32(FFT_inputbuf_1, FFT_outputbuf_1, FFT_LENGTH);
    arm_cmplx_mag_f32(FFT_inputbuf_2, FFT_outputbuf_2, FFT_LENGTH);
 
    FFT_outputbuf_1[0] = 0;
    FFT_outputbuf_2[0] = 0;
 
    for(int h=0;h<FFT_LENGTH;h++)
    {
        FFT_outputbuf_1[h]/=FFT_LENGTH/2;
        FFT_outputbuf_2[h]/=FFT_LENGTH/2;
    }
//    Printf(fft_outputbuf_1,Length);
    arm_max_f32(FFT_outputbuf_1, FFT_LENGTH/2, &Fmax_1, &Amax_pos_1);     //使用Length会让频率点在后半部分，导致频率计算错误，导致ARR->0
    arm_max_f32(FFT_outputbuf_2, FFT_LENGTH/2, &Fmax_2, &Amax_pos_2);     //使用Length会让频率点在后半部分，导致频率计算错误，导致ARR->0
    Phase_1=Phase_atan(FFT_inputbuf_1,Amax_pos_1);
    Phase_1-=0.04f;                     //根据实际情况补偿，非必要不加
    Phase_2=Phase_atan(FFT_inputbuf_2,Amax_pos_2);
    Phase=Phase_1-Phase_2;
//    printf("Phase1=%f Phase_2=%f Phase=%f\r\n",Phase_1,Phase_2,Phase);
    if(Phase>180) Phase = -360 + Phase;
    else if(Phase<-180) Phase = 360 + Phase;
}
*/

