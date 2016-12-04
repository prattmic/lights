#include "rainbow.h"

#include <stdint.h>

#include "led_string.h"

static struct color color_wheel(int pos) {
	struct color c;

	if (pos < 85) {
		c.r = pos * 3;
		c.g = 255 - pos * 3;
		c.b = 0;
	} else if (pos < 170) {
		pos -= 85;
		c.r = 255 - pos * 3;
		c.g = 0;
		c.b = pos * 3;
	} else {
		pos -= 170;
		c.r = 0;
		c.g = pos * 3;
		c.b = 255 - pos * 3;
	}

	return c;
}

void rainbow(struct color *data, uint16_t len) {
	static int curr = 0;
	curr++;

	for (int i = 0; i< len; i++) {
		int pos = ((i * 256 / len) + curr) & 255;
		data[i] = color_wheel(pos);
	}
}


