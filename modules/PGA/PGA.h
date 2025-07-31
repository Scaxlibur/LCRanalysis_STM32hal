#ifndef __PGA_H__
#define __PGA_H__

#include "dac.h"


#define DAC_MAX_VALUE ((float)4095)  // 12位DAC最大值
#define DAC_MIN_VALUE ((float)0)      // 12位DAC最小值
#define REF_VOLTAGE ((float)3300)    // 参考电压3300mV
#define DAC_CHANNEL DAC_CHANNEL_1

#define MAX_CONTROL_VOLTAGE 1500U
#define MIN_CONTROL_VOLTAGE 500U


void DACvalueSet_mv(float voltage_mV);
void VGAgainSet(float gain);

#endif