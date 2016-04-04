#include "stm32f10x.h"


/******** Используемые порты и переферия ****************/

#define TFT_Soft	GPIOB      // Порт для ногодрыга
#define TFT_PORT	GPIOA      // Порт для SPI1
#define TFT_SPI		SPI1       // Объявляем, что используем SPI1


/********* Ноги используемые для управления SPI **********/

#define PIN_LED		GPIO_Pin_12
#define PIN_RST		GPIO_Pin_1
#define PIN_DC		GPIO_Pin_0
#define PIN_CS		GPIO_Pin_2

#define PIN_SCK		GPIO_Pin_5
#define PIN_MISO	GPIO_Pin_6
#define PIN_MOSI	GPIO_Pin_7


/******* Цвета для графики ******************************/

#define RED			0xf800
#define GREEN		0x006600
#define BLUE		0x001f
#define BLACK		0x0000
#define YELLOW		0xffe0
#define WHITE		0xffff
#define CYAN		0x07ff
#define BRIGHT_RED	0xf810
#define GRAY1		0x8410
#define GRAY2		0x4208
#define PURPLE		0x03c03c
#define BROWN		0x009900


/******** Макросы для управления сбросом и чипселектом ******************/

#define TFT_CS_LOW		GPIO_WriteBit(TFT_Soft, PIN_CS, Bit_RESET);
#define TFT_CS_HIGH		GPIO_WriteBit(TFT_Soft, PIN_CS, Bit_SET);
#define TFT_DC_LOW		GPIO_WriteBit(TFT_Soft, PIN_DC, Bit_RESET);
#define TFT_DC_HIGH		GPIO_WriteBit(TFT_Soft, PIN_DC, Bit_SET);
#define TFT_RST_LOW		GPIO_WriteBit(TFT_Soft, PIN_RST, Bit_RESET);
#define TFT_RST_HIGH	GPIO_WriteBit(TFT_Soft, PIN_RST, Bit_SET);

/************ Команды для отправки в контроллер ILI9341 из даташита *****/

#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC			        0x36
#define ILI9341_PIXEL_FORMAT        0x3A
#define ILI9341_WDB			    	0x51
#define ILI9341_WCD				    0x53
#define ILI9341_RGB_INTERFACE       0xB0
#define ILI9341_FRC				    0xB1
#define ILI9341_BPC				    0xB5
#define ILI9341_DFC				    0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC				    0xF7


/********* Разрешение TFT экрана и количество пикселей ****************/

#define LCD_WIDTH       325
#define LCD_HEIGHT      247
#define LCD_PIXEL_COUNT	LCD_WIDTH * LCD_HEIGHT

/********* Размер шрифта ***********************/

#define FONT_HEIGHT 26
#define FONT_WIDTH 16



/************************** Константы для ориентации экрана ***************/
 
typedef enum {
	TM_ILI9341_Landscape,
	TM_ILI9341_Portrait
} TM_ILI9341_Orientation;

/**
 * @brief  LCD options
 * @note   Used private
 */
typedef struct {
	uint16_t width;
	uint16_t height;
	TM_ILI9341_Orientation orientation; // 1 = portrait; 0 = landscape
} TM_ILI931_Options_t;

extern TM_ILI931_Options_t ILI9341_Opts;

typedef enum {
	TM_ILI9341_Orientation_Portrait_1,  // портрет (вертикальная) 1
	TM_ILI9341_Orientation_Portrait_2,  // портрет (вертикальная) 2
	TM_ILI9341_Orientation_Landscape_1, // альбомная 1
	TM_ILI9341_Orientation_Landscape_2  // альбомная 2
} TM_ILI9341_Orientation_t;

/********************************************************************************************************************************************/
/************************** Список используемых функций *************************************************************************************/

void TFT_init(void);
void TFT_led(int state);
void TFT_Fill(uint16_t color);
void TFT_DrawString(uint32_t x, uint32_t y, char *str, uint32_t ground, uint32_t color);
void TFT_Orientation(TM_ILI9341_Orientation_t orientation);
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void TFT_drawCircle(int poX, int poY, int r, int color);
void TFT_drawLine(int x0, int y0, int x1, int y1, int color);
void TFT_drawRectangle(int poX, int poY, int length, int width, int color);
void TFT_fillRectangle (int poX, int poY, int length, int width, int color);

void TFT_Image(unsigned char const *img); 

void TFT_DisplayOn(void);
void TFT_DisplayOff(void);

/********************************************************************************************************************************************/
void TFT_sendByte(uint8_t data);
void TFT_sendCMD(int index);
void TFT_sendDATA(int data);
void TFT_sendWord(int data);
void TFT_setCol(int StartCol, int EndCol);
void TFT_setPage(int StartPage, int EndPage);
void TFT_setXY(int poX, int poY);
void TFT_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
uint32_t TFT_Putchar(uint32_t x, uint32_t y, char c, uint32_t ground, uint32_t color);



