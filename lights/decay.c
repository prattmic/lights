#include "decay.h"

#include <stdint.h>

#include "led_string.h"

static void subfloor(struct color *data, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		int16_t tmpr = data[i].r - data[i].r/6;
		if(tmpr < 0) {
			tmpr = 0;
		}

		int16_t tmpg = data[i].g - data[i].g/6;
		if(tmpg < 0) {
			tmpg = 0;
		}

		int16_t tmpb = data[i].b - data[i].b/6;
		if(tmpb < 0) {
			tmpb = 0;
		}

		data[i].r = tmpr;
		data[i].g = tmpg;
		data[i].b = tmpb;
	}

}

void decay(struct color *data, uint16_t len)
{
	struct color scratch[len];

	// Shift & initial decay into buffer
	for(uint64_t i = 1; i < len; i++) {
		scratch[i].r = data[i-1].r;
		scratch[i].g = data[i-1].g;
		scratch[i].b = data[i-1].b;
	}

	// Add buf back in
	for(uint64_t i = 1; i < len; i++) {
		data[i].r = scratch[i].r;
		data[i].g = scratch[i].g;
		data[i].b = scratch[i].b;
	}
	data[0].r = data[len-1].r;
	data[0].g = data[len-1].g;
	data[0].b = data[len-1].b;

	// Decay (without underflowing)
	subfloor(data, len);
}
