#include "snake.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "color_wheel.h"
#include "led_string.h"

static struct color random_color(void) {
	return color_wheel(rand() % 256);
}

static struct color fade(struct color c, int i) {
	struct color faded;

	if (10*i < c.r) {
		faded.r = c.r - 10*i;
	} else {
		faded.r = 0;
	}

	if (10*i < c.g) {
		faded.g = c.g - 10*i;
	} else {
		faded.g = 0;
	}

	if (10*i < c.b) {
		faded.b = c.b - 10*i;
	} else {
		faded.b = 0;
	}

	return faded;
}

void snake(uint32_t call, struct color *data, uint16_t len)
{
	if (call % 10) {
		return;
	}

	bool empty = true;
	for (int i = 0; i < len; i++) {
		if (data[i].r || data[i].g || data[i].b) {
			empty = false;
			break;
		}
	}

	if (empty) {
		struct color init = random_color();
		for (int i = 0; i < len; i++) {
			data[i] = fade(init, i);
		}
	}

	struct color original[len];
	for (int i = 0; i < len; i++) {
		original[i] = data[i];
	}

	for (int i = 0; i < len; i++) {
		if (i >= (len-1)) {
			data[i] = original[0];
		} else {
			data[i] = original[i+1];
		}
	}
}
