#include "TIM_Lib.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"


int sec;

/**************** Настройка таймера и прерывания для отсчета 1 секунды *****/

void TIM_Init (void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);         // включаем тактирование таймера

	TIM_TimeBaseInitTypeDef Tim_Sec;                             // создаём имя для настроек таймера
	Tim_Sec.TIM_CounterMode = TIM_CounterMode_Up;                // Выбираем режим работы таймера (по переполнению)
	Tim_Sec.TIM_Prescaler = 24000 - 1;                           // предделитель, 1 тик = 1 мс
	Tim_Sec.TIM_Period = 1000;                                   // период до скольки считаем
	TIM_TimeBaseInit(TIM3, &Tim_Sec);                            // Засовываем настройки в регистры

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                   // разрешаем таймеру прерывания

	TIM_Cmd(TIM3, ENABLE);                                       // Запускаем таймер

	NVIC_EnableIRQ(TIM3_IRQn);                                   // Разрешаем обработку прерывания от таймера 3
}

/************ Подсчет 1 секунды ***************/

void TIM3_IRQHandler(void)
{

  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                      // Очищаем флаг прерывания
  sec=sec+1;
  
}