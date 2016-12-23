#ifndef LIGHTS_EFFECTS_H_
#define LIGHTS_EFFECTS_H_

#include <stdint.h>

#include "led_string.h"

typedef void (*effect_func)(uint32_t, struct color *, uint16_t);

effect_func random_effect(void);

#endif /* LIGHTS_EFFECTS_H_ */
