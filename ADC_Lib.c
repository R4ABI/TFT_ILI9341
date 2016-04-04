#include "ADC_Lib.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"

uint16_t data_adc;
float volt;

void ADC_ini(void)
{
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 // настройки ADC
 ADC_InitTypeDef ADC_InitStructure;
 ADC_StructInit(&ADC_InitStructure);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // режим работы - одиночный, независимый
 ADC_InitStructure.ADC_ScanConvMode = DISABLE; // не сканировать каналы, просто измерить один канал
 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // однократное измерение
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // без внешнего триггера
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //выравнивание битов результат - прижать вправо
 ADC_InitStructure.ADC_NbrOfChannel = 1; //количество каналов - одна штука
 ADC_Init(ADC1, &ADC_InitStructure);
 ADC_Cmd(ADC1, ENABLE);
  
 // настройка канала
 ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
  
 // калибровка АЦП
 ADC_ResetCalibration(ADC1);
 while (ADC_GetResetCalibrationStatus(ADC1));
 ADC_StartCalibration(ADC1);
 while (ADC_GetCalibrationStatus(ADC1)); 
}

uint16_t get_adc_value()
{
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);
 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
 return ADC_GetConversionValue(ADC1);
}
