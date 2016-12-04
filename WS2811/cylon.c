#include "cylon.h"

#include <stdbool.h>
#include <stdint.h>

#include "led_string.h"

void cylon(struct color *data, uint16_t len)
{
	static int center = 0;
	static bool up = true;

	for (int i = 0; i < len; i++) {
		if (i == center-2) {
			data[i].r = 32;
		} else if (i == center-1) {
			data[i].r = 64;
		} else if (i == center) {
			data[i].r = 128;
		} else if (i == center+1) {
			data[i].r = 64;
		} else if (i == center+2) {
			data[i].r = 32;
		} else {
			data[i].r = 0;
		}
		data[i].g = 0;
		data[i].b = 0;
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
