#include "PGA.h"

/**
 * VCA821的增益/控制电压关系
 * x是控制电压，单位mV
 * y是增益，单位1
 * 四次多项式  y = 4E-10x^4 - 2E-06x^3 + 0.0026x^2 - 1.607x + 336.95
 * 六次多项式  y = -1E-15x6 + 9E-12x5 - 2E-08x4 + 3E-05x3 - 0.0185x2 + 6.2379x - 840.86
 * 
 * x是增益，单位1
 * y是控制电压，单位mV
 * 分段拟合
 *  增益2-12  y = -1.2392x^2 + 35.487x + 441.26
 *  增益12-49 y = 7.6643x + 611.32
 */

/**
 * @brief 设定ADC输出为对应幅度，单位mV
 * @param voltage_mV 单位mV
 */
void DACvalueSet_mv(float voltage_mV)
{
    float value = voltage_mV * (DAC_MAX_VALUE / REF_VOLTAGE); // 计算DAC值
    if(value > DAC_MAX_VALUE) {
        value = DAC_MAX_VALUE;
    }else if(value < DAC_MIN_VALUE) {
        value = DAC_MIN_VALUE;
    }
    HAL_DAC_Stop(&hdac, DAC_CHANNEL);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
}

/**
 * @brief 设定ADC输出为对应增益
 * @param gain 单位1
 */
void VGAgainSet(float gain)
{
    double control_voltage;
    if(gain < 12 || gain > 2)
    {
        control_voltage = -1.2392 * gain * gain + 35.487 * gain + 441.26; // 计算控制电压（增益2-12）
    }else if(gain >= 12)
    {
        control_voltage = 7.6643 * gain + 611.32; // 计算控制电压（增益12-49）
    }


    // double control_voltage;
    // double control_voltage1 = (0.0000000004 * gain * gain * gain * gain);
    // control_voltage = control_voltage1 - (0.00002 * gain * gain * gain) + (0.0026 * gain * gain) - (1.607 * gain) + 336.95; // 计算控制电压（四次拟合）
    // control_voltage = (-1E-15 * gain * gain * gain * gain * gain * gain); // 计算控制电压（六次拟合）
    if(control_voltage > MAX_CONTROL_VOLTAGE) {
        control_voltage = MAX_CONTROL_VOLTAGE;
    } else if(control_voltage < MIN_CONTROL_VOLTAGE) {
        control_voltage = MIN_CONTROL_VOLTAGE;
    }
    DACvalueSet_mv(control_voltage);
}
