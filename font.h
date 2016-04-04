#include <stm32f10x.h>

typedef struct {
	uint16_t width;
	uint16_t height;
	unsigned short const *data_table;
} font_type;

