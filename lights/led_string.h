#ifndef WS2811_LED_STRING_H_
#define WS2811_LED_STRING_H_

#include <stdint.h>

/*
 * Single pixel RGB data structure. Make an array out of this to store RGB data
 * for a string.
 */
struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

void setup_spi(void);
void update_string(struct color *data, uint16_t len);

#endif /* WS2811_LED_STRING_H_ */
