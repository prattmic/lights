#include "rainbow.h"

#include <stdint.h>

#include "color_wheel.h"
#include "led_string.h"

void rainbow(uint32_t call, struct color *data, uint16_t len) {
	for (int i = 0; i< len; i++) {
		int pos = ((i * 256 / len) + call) & 255;
		data[i] = color_wheel(pos);
	}
}
