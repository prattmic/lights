#ifndef WS2811_DECAY_H_
#define WS2811_DECAY_H_

#include <stdint.h>

#include "led_string.h"

void decay(struct color *data, uint16_t len);

#endif /* WS2811_DECAY_H_ */
