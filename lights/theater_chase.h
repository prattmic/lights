#ifndef WS2811_THEATER_CHASE_H_
#define WS2811_THEATER_CHASE_H_

#include <stdint.h>

#include "led_string.h"

void theater_chase(uint32_t call, struct color *data, uint16_t len);
void christmas_chase(uint32_t call, struct color *data, uint16_t len);

#endif /* WS2811_THEATER_CHASE_H_ */
