#include "theater_chase.h"

#include <stdbool.h>
#include <stdint.h>

#include "led_string.h"

void theater_chase(uint32_t call, struct color *data, uint16_t len)
{
	static int update = 0;

	/* Only update every 100ms */
	if (call % 10) {
		return;
	}

	/* Turn every third LED on, rotating the starting LED. */
	for (int i = 0; i < len; i++) {
		data[i].r = 0;
		data[i].g = 0;
		data[i].b = 0;

		if (i%5 == update%5) {
			data[i].b = 128;
		}
	}

	update++;
}

void christmas_chase(uint32_t call, struct color *data, uint16_t len)
{
	static int update = 0;

	/* Only update every 100ms */
	if (call % 10) {
		return;
	}

	for (int i = 0; i < len; i++) {
		data[i].r = 0;
		data[i].g = 0;
		data[i].b = 0;

		if (i%5 == update%5) {
			data[i].g = 128;
		}

		if (i%5 == (update%5 + 2)%5) {
			data[i].r = 128;
		}
	}

	update++;
}
