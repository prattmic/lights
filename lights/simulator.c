#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "effects.h"
#include "led_string.h"

#define NUM_LEDS 50

void print_leds(struct color *data, size_t num) {
	for (int i = 0; i < num; i++) {
		if (i != 0) {
			printf(",");
		}
		printf("%d,%d,%d", data[i].r, data[i].g, data[i].b);
	}
	printf("\n");
	fflush(stdout);
}

int main(void) {
	struct color led_data[NUM_LEDS] = {0};
	effect_func effect = random_effect();

	for (uint32_t i = 0; ; i++) {
		/* New effect every 10s */
		if ((i % 1000) == 0) {
			memset(led_data, 0, sizeof(led_data));
			effect = random_effect();
		}

		/* Step the effect */
		effect(i, led_data, NUM_LEDS);

		/* Print the step */
		print_leds(led_data, NUM_LEDS);

		/* Run every 10ms */
		usleep(10000);
	}
}
