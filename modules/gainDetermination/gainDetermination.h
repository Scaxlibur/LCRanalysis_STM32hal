#ifndef __GAINDETERMINATION_H__
#define __GAINDETERMINATION_H__

#include "arm_math.h"

#include "AD9910.h"

#define DEFUALT_BACK_GAIN 4.0f	// AD9910后的默认增益
#define AD9910_SYSTEM_ERROR	4	// 系统误差，幅度稳定加一点	500mv以上输出才加
#define AD9910_MAX_Vpp 875
#define AD9910_MIN_Vpp 200

#define KNOWN_MODLE_OUT_PUT_Vpp 

void DDSoriginAmpSet(uint16_t targetAmp);
double knownModelGain(uint32_t inputFreq);
void knownModelOutputSet(uint16_t targetAmp, unsigned long freq);


#endif
