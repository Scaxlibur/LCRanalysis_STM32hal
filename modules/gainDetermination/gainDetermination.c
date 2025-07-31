#include "gainDetermination.h"

/**
 * @brief 根据已知模型的系统函数求解对应频率的增益
 * @param inputFreq 输入频率
 * @return 对应频率的增益值
 */
double knownModelGain(uint32_t inputFreq)
{
    double omega = 2 * PI * inputFreq; // 角频率
    double gain = 0;
    gain = 5/(pow( (1e-16) * omega * omega * omega * omega + (1e-8)*7 * omega * omega + 1,0.5));
    return gain;
}

/**
 * @brief 设置AD9910的幅度输出
 * @param targetAmp 目标幅度，单位mV
 */
void DDSoriginAmpSet(uint16_t targetAmp)
{
    if(targetAmp > 500)
        targetAmp += AD9910_SYSTEM_ERROR; // 补偿系统误差
    if(targetAmp > AD9910_MAX_Vpp)
        targetAmp = AD9910_MAX_Vpp;
    else if(targetAmp < AD9910_MIN_Vpp)
        targetAmp = AD9910_MIN_Vpp;
    Write_Amplitude(targetAmp);
}

/**
 * @brief 根据已知模型和频率设置对应幅度输出
 * @param targetAmp 目标幅度，单位mV
 * @param freq 模块输入频率，单位Hz
 */
void knownModelOutputSet(uint16_t targetAmp, unsigned long freq)
{
    double modleGain = knownModelGain(freq);    // 已知模块的增益
    double DDSoutput = targetAmp / modleGain / DEFUALT_BACK_GAIN;   // 计算AD9910输出幅度
    Freq_convert(freq);
    DDSoriginAmpSet((uint16_t)DDSoutput); // 设置AD9910输出幅度
}
