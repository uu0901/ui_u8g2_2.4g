#ifndef __AD_H
#define __AD_H

#define ROCK1_Y_ADC_Channel		ADC_Channel_8
#define ROCK1_X_ADC_Channel		ADC_Channel_9

#define ROCK2_Y_ADC_Channel		ADC_Channel_1
#define ROCK2_X_ADC_Channel		ADC_Channel_2

#define BAT_ADC_Channel		    ADC_Channel_0
void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);

#endif
