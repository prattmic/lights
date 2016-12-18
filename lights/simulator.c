#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cylon.h"
#include "decay.h"
#include "led_string.h"
#include "rainbow.h"
#include "theater_chase.h"

#define NUM_LEDS 50

typedef void (*effect_func)(struct color *, uint16_t);

static const effect_func effects[] = {
	cylon,
	/* Does nothing if it starts with all zeroes */
	/* decay, */
	rainbow,
	theater_chase,
	christmas_chase,
};

static effect_func random_effect(void) {
	int i = rand() % sizeof(effects)/sizeof(effects[0]);
	return effects[i];
}

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

	for (int i = 0; ; i++) {
		/* New effect every 10s */
		if ((i % 1000) == 0) {
			memset(led_data, 0, sizeof(led_data));
			effect = random_effect();
		}

		/* Step the effect */
		effect(led_data, NUM_LEDS);

		/* Print the step */
		print_leds(led_data, NUM_LEDS);

		/* Run every 10ms */
		usleep(10000);
	}
}
