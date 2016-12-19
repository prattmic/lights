#include "lightning.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "led_string.h"

/*
 * Write the gradient of src to dst.
 *
 * Behaves the same as numpy.gradient.
 */
static void gradient(uint8_t *src, int16_t *dst, uint16_t len)
{
	for (int i = 0; i < len; i++) {
		if (i == 0) {
			dst[i] = (int16_t)src[i+1] - src[i];
		} else if (i == (len - 1)) {
			dst[i] = (int16_t)src[i] - src[i-1];
		} else {
			dst[i] = ((int16_t)src[i+1] - src[i-1]) / 2;
		}
	}
}

void lightning(struct color *data, uint16_t len)
{
	static int call = 0;

	/* Only update every 100ms */
	if (call++ % 10) {
		return;
	}

	uint8_t brightness[len];

	for (int i = 0; i < len; i++) {
		brightness[i] = data[i].r;
	}

	if ((rand() % 10) == 0) {
		brightness[rand() % len] = 255;
	}

	int16_t grad[len];
	gradient(brightness, grad, len);

	for (int i = 0; i < len; i++) {
		brightness[i] = brightness[i] / 8;

		if (grad[i] > 0 && i < (len-1)) {
			brightness[i] += brightness[i+1] * 0.7;
		} else if (grad[i] < 0 && i > 0) {
			brightness[i] += brightness[i-1] * 0.7;
		}
	}

	for (int i = 0; i < len; i++) {
		data[i].r = brightness[i];
		data[i].g = brightness[i];
		data[i].b = brightness[i];
	}
}
