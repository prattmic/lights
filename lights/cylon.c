#include "cylon.h"

#include <stdbool.h>
#include <stdint.h>

#include "led_string.h"

void cylon(uint32_t call, struct color *data, uint16_t len)
{
	static int center = 0;
	static bool up = true;

	/* Update at half speed */
	if (call % 2) {
		return;
	}

	for (int j = 0; j < len; j++) {
		if (j == center-2) {
			data[j].r = 32;
		} else if (j == center-1) {
			data[j].r = 64;
		} else if (j == center) {
			data[j].r = 128;
		} else if (j == center+1) {
			data[j].r = 64;
		} else if (j == center+2) {
			data[j].r = 32;
		} else {
			data[j].r = 0;
		}
		data[j].g = 0;
		data[j].b = 0;
	}

	/* Update center */
	if (up) {
		center++;
		if (center >= len) {
			up = false;
		}
	} else {
		center--;
		if (center < 0) {
			up = true;
		}
	}
}
