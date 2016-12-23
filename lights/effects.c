#include "effects.h"

#include <stdlib.h>

#include "cylon.h"
#include "decay.h"
#include "lightning.h"
#include "rainbow.h"
#include "theater_chase.h"

static const effect_func effects[] = {
	cylon,
	/* Does nothing if it starts with all zeroes */
	/* decay, */
	rainbow,
	theater_chase,
	christmas_chase,
	lightning,
};

effect_func random_effect(void) {
	int i = rand() % sizeof(effects)/sizeof(effects[0]);
	return effects[i];
}
