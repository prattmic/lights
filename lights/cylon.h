#ifndef WS2811_CYLON_H_
#define WS2811_CYLON_H_

#include <stdint.h>

#include "led_string.h"

void cylon(uint32_t call, struct color *data, uint16_t len);

#endif /* WS2811_CYLON_H_ */
