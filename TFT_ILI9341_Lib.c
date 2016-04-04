#include "TFT_ILI9341_Lib.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

#include "font.h"

extern const font_type TimesNewRoman;
TM_ILI931_Options_t ILI9341_Opts;

int i, j;

/****************************************************************************************/
/******************************* Отправка данных и команд по SPI ************************/
/****************************************************************************************/

/************ Функция задержки для TFT ********************/

void delay_us (uint32_t us) {
	volatile uint32_t n = 5 * us;

	while (--n > 0);
}

/********* Отправка байта в SPI и проверка флага о завершение отправки *******/

void TFT_sendByte (uint8_t data) {

	while (SPI_I2S_GetFlagStatus (TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData (SPI1, data);

}

/********** Отправка команды в дисплей ************************************/

void TFT_sendCMD (int index) {

	while (SPI_I2S_GetFlagStatus (TFT_SPI, SPI_I2S_FLAG_BSY) == SET);
	TFT_CS_HIGH;
	TFT_DC_LOW;
	TFT_CS_LOW;
	TFT_sendByte (index);

}

/******************** Отправка данных в дисплей ***************************/

void TFT_sendDATA (int data) {

	while (SPI_I2S_GetFlagStatus (TFT_SPI, SPI_I2S_FLAG_BSY) == SET);
	TFT_DC_HIGH;
	TFT_sendByte (data);

}



/******************* Функция отправки слова на дисплей *********************/

void TFT_sendWord (int data) {
	while (SPI_I2S_GetFlagStatus (TFT_SPI, SPI_I2S_FLAG_BSY) == SET);
	TFT_DC_HIGH;

	TFT_sendByte (data >> 8);
	TFT_sendByte (data & 0x00ff);
}

/************* Функция инициализации дисплея на контроллере ILI9341 **********/

void TFT_init () {


	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);


	GPIO_InitTypeDef soft;
	GPIO_InitTypeDef gpio;
	SPI_InitTypeDef spi;


	gpio.GPIO_Pin = PIN_SCK | PIN_MOSI | PIN_MISO;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init (TFT_PORT, &gpio);


	soft.GPIO_Pin = PIN_LED | PIN_DC | PIN_RST | PIN_CS;
	soft.GPIO_Speed = GPIO_Speed_50MHz;
	soft.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init (TFT_Soft, &soft);

	//SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init (TFT_SPI, &spi);

	SPI_Cmd (TFT_SPI, ENABLE);

	TFT_RST_LOW;
	delay_us (100);
	TFT_RST_HIGH;

	/* Delay for RST response */
	delay_us (100);

	/* Software reset */
	TFT_sendCMD (ILI9341_RESET);
	delay_us (100);

	TFT_sendCMD (ILI9341_POWERA);
	TFT_sendDATA (0x39);
	TFT_sendDATA (0x2C);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x34);
	TFT_sendDATA (0x02);
	TFT_sendCMD (ILI9341_POWERB);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0xC1);
	TFT_sendDATA (0x30);
	TFT_sendCMD (ILI9341_DTCA);
	TFT_sendDATA (0x85);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x78);
	TFT_sendCMD (ILI9341_DTCB);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x00);
	TFT_sendCMD (ILI9341_POWER_SEQ);
	TFT_sendDATA (0x64);
	TFT_sendDATA (0x03);
	TFT_sendDATA (0x12);
	TFT_sendDATA (0x81);
	TFT_sendCMD (ILI9341_PRC);
	TFT_sendDATA (0x20);
	TFT_sendCMD (ILI9341_POWER1);
	TFT_sendDATA (0x23);
	TFT_sendCMD (ILI9341_POWER2);
	TFT_sendDATA (0x10);
	TFT_sendCMD (ILI9341_VCOM1);
	TFT_sendDATA (0x3E);
	TFT_sendDATA (0x28);
	TFT_sendCMD (ILI9341_VCOM2);
	TFT_sendDATA (0x86);
	TFT_sendCMD (ILI9341_MAC);
	TFT_sendDATA (0x48);
	TFT_sendCMD (ILI9341_PIXEL_FORMAT);
	TFT_sendDATA (0x55);
	TFT_sendCMD (ILI9341_FRC);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x18);
	TFT_sendCMD (ILI9341_DFC);
	TFT_sendDATA (0x08);
	TFT_sendDATA (0x82);
	TFT_sendDATA (0x27);
	TFT_sendCMD (ILI9341_3GAMMA_EN);
	TFT_sendDATA (0x00);
	TFT_sendCMD (ILI9341_COLUMN_ADDR);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0xEF);
	TFT_sendCMD (ILI9341_PAGE_ADDR);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x01);
	TFT_sendDATA (0x3F);
	TFT_sendCMD (ILI9341_GAMMA);
	TFT_sendDATA (0x01);
	TFT_sendCMD (ILI9341_PGAMMA);
	TFT_sendDATA (0x0F);
	TFT_sendDATA (0x31);
	TFT_sendDATA (0x2B);
	TFT_sendDATA (0x0C);
	TFT_sendDATA (0x0E);
	TFT_sendDATA (0x08);
	TFT_sendDATA (0x4E);
	TFT_sendDATA (0xF1);
	TFT_sendDATA (0x37);
	TFT_sendDATA (0x07);
	TFT_sendDATA (0x10);
	TFT_sendDATA (0x03);
	TFT_sendDATA (0x0E);
	TFT_sendDATA (0x09);
	TFT_sendDATA (0x00);
	TFT_sendCMD (ILI9341_NGAMMA);
	TFT_sendDATA (0x00);
	TFT_sendDATA (0x0E);
	TFT_sendDATA (0x14);
	TFT_sendDATA (0x03);
	TFT_sendDATA (0x11);
	TFT_sendDATA (0x07);
	TFT_sendDATA (0x31);
	TFT_sendDATA (0xC1);
	TFT_sendDATA (0x48);
	TFT_sendDATA (0x08);
	TFT_sendDATA (0x0F);
	TFT_sendDATA (0x0C);
	TFT_sendDATA (0x31);
	TFT_sendDATA (0x36);
	TFT_sendDATA (0x0F);
	TFT_sendCMD (ILI9341_SLEEP_OUT);

	delay_us (500);

	TFT_sendCMD (ILI9341_DISPLAY_ON);
	TFT_sendCMD (ILI9341_GRAM);


}

/******************* Функция включения подсветки ************************/

void TFT_led (int state) {
	if (state)
		GPIO_WriteBit (TFT_Soft, PIN_LED, Bit_SET);
	else
		GPIO_WriteBit (TFT_Soft, PIN_LED, Bit_RESET);
}

/********************* Функция установки хз чего **************/

void TFT_setCol (int StartCol, int EndCol) {
	TFT_sendCMD (0x2A);
	TFT_sendWord (StartCol);
	TFT_sendWord (EndCol);
}

/******************** Функция установки страницы *************************/

void TFT_setPage (int StartPage, int EndPage) {
	TFT_sendCMD (0x2B);
	TFT_sendWord (StartPage);
	TFT_sendWord (EndPage);
}

/*********** Функция установки координаты на дисплее *********************/

void TFT_setXY (int poX, int poY) {
	TFT_setCol (poX, poX);
	TFT_setPage (poY, poY);
	TFT_sendCMD (0x2c);
}


/************************** Функция установки координаты курсора на TFT *********************/
/********************************************************************************************/

void TFT_SetCursorPosition (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	TFT_sendCMD (ILI9341_COLUMN_ADDR);
	TFT_sendDATA (x1 >> 8);
	TFT_sendDATA (x1 & 0xFF);
	TFT_sendDATA (x2 >> 8);
	TFT_sendDATA (x2 & 0xFF);

	TFT_sendCMD (ILI9341_PAGE_ADDR);
	TFT_sendDATA (y1 >> 8);
	TFT_sendDATA (y1 & 0xFF);
	TFT_sendDATA (y2 >> 8);
	TFT_sendDATA (y2 & 0xFF);
}

/********** Заливка экрана цветом **************/

void TFT_Fill (uint16_t color) {
	unsigned int n, i, j;
	i = color >> 8;
	j = color & 0xFF;
	TFT_SetCursorPosition (0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

	TFT_sendCMD (ILI9341_GRAM);

	for (n = 0; n < LCD_PIXEL_COUNT; n++) {
		TFT_sendDATA (i);
		TFT_sendDATA (j);
	}
}

/*********************************** Написание строки **********************************/

uint32_t TFT_Putchar (uint32_t x, uint32_t y, char c, uint32_t ground, uint32_t color) {
	uint32_t i, j;
	unsigned short Data;

	uint32_t offset = (c - 32)*TimesNewRoman.height;
	uint16_t width = TimesNewRoman.width;

	for (i = 0; i < TimesNewRoman.height; i++) {

		Data = TimesNewRoman.data_table [offset + i];


		for (j = 0; j < width; j++) {
			if ((Data << j) & 0x8000) {
				TFT_DrawPixel (x + j, (y + i), color);  // цвет текста
			} else {
				TFT_DrawPixel (x + j, (y + i), ground);  // цвет фона под текстом
			}
		}
	}

	return x + width;
}


/***************************** Рисование цветного пикселя (вариант №2) **********************************/

void TFT_DrawPixel (uint16_t x, uint16_t y, uint16_t color) {
	TFT_SetCursorPosition (x, y, x, y);
	TFT_sendCMD (ILI9341_GRAM);
	TFT_sendDATA (color >> 8);
	TFT_sendDATA (color & 0xFF);
}

/*************************** Рисование строки **********************************************************/

void TFT_DrawString (uint32_t x, uint32_t y, char *str, uint32_t ground, uint32_t color) {
	while (*str) {
		x = TFT_Putchar (x, y, *str++, ground, color);
	}
}

/*********** Функция положения и вращение экрана *************/

void TFT_Orientation (TM_ILI9341_Orientation_t orientation) {
	TFT_sendCMD (ILI9341_MAC);
	if (orientation == TM_ILI9341_Orientation_Portrait_1) {
		TFT_sendDATA (0x58);
	} else if (orientation == TM_ILI9341_Orientation_Portrait_2) {
		TFT_sendDATA (0x88);
	} else if (orientation == TM_ILI9341_Orientation_Landscape_1) {
		TFT_sendDATA (0x28);
	} else if (orientation == TM_ILI9341_Orientation_Landscape_2) {
		TFT_sendDATA (0xE8);
	}

	if (orientation == TM_ILI9341_Orientation_Portrait_1 || orientation == TM_ILI9341_Orientation_Portrait_2) {
		ILI9341_Opts.width = LCD_WIDTH;
		ILI9341_Opts.height = LCD_HEIGHT;
		ILI9341_Opts.orientation = TM_ILI9341_Portrait;
	} else {
		ILI9341_Opts.width = LCD_WIDTH;
		ILI9341_Opts.height = LCD_HEIGHT;
		ILI9341_Opts.orientation = TM_ILI9341_Landscape;
	}
}

/********************* Функция включения и выключения дисплея *************/

void TFT_DisplayOn (void) {
	TFT_sendCMD (ILI9341_DISPLAY_ON);
}

void TFT_DisplayOff (void) {
	TFT_sendCMD (ILI9341_DISPLAY_OFF);
}

/************ Рисование круга *************************/

void TFT_drawCircle (int poX, int poY, int r, int color) {
	int x = -r, y = 0, err = 2 - 2 * r, e2;
	do {
		TFT_DrawPixel (poX - x, poY + y, color);
		TFT_DrawPixel (poX + x, poY + y, color);
		TFT_DrawPixel (poX + x, poY - y, color);
		TFT_DrawPixel (poX - x, poY - y, color);
		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x * 2 + 1;
	}
	while (x <= 0);
}

/*************** Рисуем линию произвольную ************/

void TFT_drawLine (int x0, int y0, int x1, int y1, int color) {
	int dx = (x1 - x0), sx = x0<x1 ? 1 : -1;
	int dy = -(y1 - y0), sy = y0<y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */
	for (;;) { /* loop */
		TFT_DrawPixel (x0, y0, color);
		e2 = 2 * err;
		if (e2 >= dy) { /* e_xy+e_x > 0 */
			if (x0 == x1) break;
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			if (y0 == y1) break;
			err += dx;
			y0 += sy;
		}
	}
}

/************** Рисуем прямоугольник *********************/

void TFT_drawRectangle (int poX, int poY, int length, int width, int color) {
	TFT_drawLine (poX, poY, poX + length, poY, color);
	TFT_drawLine (poX, poY + width, poX + length, poY + width, color);

	TFT_drawLine (poX, poY, poX, poY + width, color);
	TFT_drawLine (poX + length, poY, poX + length, poY + width, color);

}

/******************* Рисуем залитый прямугольник **************/

void TFT_fillRectangle (int poX, int poY, int length, int width, int color) {
	int i;
	for (i = 0; i<width; i++) {
		TFT_drawLine (poX, poY + i, poX + length, poY + i, color);

	}
}

/**************** Рисуем картинку *******************************/

void TFT_Image (unsigned char const *img) {
	uint32_t n;

	TFT_SetCursorPosition (0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

	TFT_sendCMD (ILI9341_GRAM);

	for (n = 0; n < LCD_PIXEL_COUNT; n++) {
		u8 color = *img;
		img++;
		TFT_sendDATA (*img);
		TFT_sendDATA (color);
		img++;
	}
}
