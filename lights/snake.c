#include "snake.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "led_string.h"

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
		for (int i = 0; i < len; i++) {
			int val = 255 - 10*i;
			if (val < 0) {
				val = 0;
			}
			data[i].r = val;
			data[i].g = 0;
			data[i].b = val;
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
