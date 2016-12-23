#ifndef WS2811_LIGHTNING_H_
#define WS2811_LIGHTNING_H_

#include <stdint.h>

#include "led_string.h"

void lightning(uint32_t call, struct color *data, uint16_t len);

#endif /* WS2811_LIGHTNING_H_ */
