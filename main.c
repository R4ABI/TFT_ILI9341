#include "stm32f10x_conf.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_adc.h"
#include <stdio.h>

#include "TFT_ILI9341_Lib.h"
#include "TIM_Lib.h"
#include "string.h"
#include "ADC_Lib.h"

#define sum strcat

/********************** Функции используемы для работы **********************************************************/
/****************************************************************************************************************/

/*************************************** Тактирование процессора **********************************/

void RCC_Configuration (void);
ErrorStatus HSEStartUpStatus;
RCC_ClocksTypeDef RCC_Clocks;

void RCC_Configuration (void) {

	RCC_DeInit ();                                               // Выполняем сброс reset
	RCC_HSEConfig (RCC_HSE_ON);                                  // Включаем тактирование HSE (от кварца)
	HSEStartUpStatus = RCC_WaitForHSEStartUp ();                 // Ждем пока частота кварца стабилизируется

	if (HSEStartUpStatus == SUCCESS)                            // Если все отлично, то переходим на кварц
	{

		RCC_HCLKConfig (RCC_SYSCLK_Div1);                        // Выставляет делитель в 1 (Div1) и системная частота становится 24 МГц
		RCC_PCLK2Config (RCC_HCLK_Div1);                         // Запитываем APB2 от тактовой частоты PLL в 24 МГц
		RCC_PCLK1Config (RCC_HCLK_Div1);                         // Запитываем APB1 от тактовой частоты PLL в 24 МГц
		RCC_ADCCLKConfig (RCC_PCLK2_Div2);                       // Ставим делитель на 2 (Div2) и получаем частоту 12 МГц вместо входящей 72 МГц
		RCC_PLLConfig (RCC_PLLSource_PREDIV1, RCC_PLLMul_3);     // Устанавливаем множитель частоты 3, получаем 8 МГц * 3 = 24 МГц на PLL

		RCC_PLLCmd (ENABLE);                                     // Включаем PLL

		while (RCC_GetFlagStatus (RCC_FLAG_PLLRDY) == RESET) {}  // Ждем пока PLL устаканится

		RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);              // Выбираем PLL как источник тактового сигнала


		while (RCC_GetSYSCLKSource () != 0x08) {}                // Ждем пока PLL установится как источник тактирования
	}
}

/******************************** Преобразование целого в строку **********************************/

char* intToString (uint32_t value) {
	static char buffer [11] = { 0 };
	sprintf (buffer, "%d", value);
	return buffer;
}

/******************************* Преобразование вещественного в строку ****************************/

char* floatToString (float value) {
	static char buffer [11] = { 0 };
	sprintf (buffer, "%.2f", value);
	return buffer;
}

/******************** Функция выврда рабочаей частоты PLL **********************/

void Print_SysClock (uint8_t x, uint8_t y) {

	char SysCrClck [25] = "CoreClock= "; //Создаю базовую строку
	sum (sum (SysCrClck, intToString (SystemCoreClock / 1000000)), "MHz"); //объединяю три строки в одну
	TFT_DrawString (x, y, SysCrClck, BLACK, BLUE);

}

/************************** Основное тело программы ***********************************************/
/**************************************************************************************************/

int main (void) {

	/************ Инициализация тактирования ************/

	RCC_GetClocksFreq (&RCC_Clocks);
	RCC_Configuration ();
	RCC_GetClocksFreq (&RCC_Clocks);

	__enable_irq ();

	/************** Инициализация TFT экрана *************/
	TFT_init ();
	TIM_Init ();

	TFT_led (1);
	TFT_Orientation (TM_ILI9341_Orientation_Landscape_1);
	TFT_Fill (BLACK);

	/*********** Инициализация АЦП *************************/

	ADC_ini ();

	/********** Создание необходимых переменных ************/

	extern int sec;
	sec = 0;

	extern uint16_t data_adc;
	extern uint16_t get_adc_value ();
	extern float volt;
	float presc = 0.0008;

	/***********************************************************************************/
	/***********************************************************************************/
	/***********************************************************************************/

	/****** Текст на экран информации *****************/

	Print_SysClock (10, 150);

	while (1) {

		data_adc = get_adc_value ();
		float volt = ((float)data_adc * presc);

		char ADC_result [25] = "ADC_Result= ";               // Создаю базовую строку
		sum (sum (ADC_result, floatToString (volt)), " V");    // Объединяю три строки в одну
		TFT_DrawString (10, 50, ADC_result, BLACK, PURPLE);


		char str [20];
		sprintf (str, "Second = %d", sec);
		TFT_DrawString (10, 100, str, BLACK, PURPLE);


	}

}
