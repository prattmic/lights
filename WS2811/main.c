/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/timer.h>
#include <stdlib.h>
#include <math.h>

#include "led_string.h"

#define N_LEDS 50

void shiftdecay(color *data, color *buf, uint16_t len);

static void setup_main_clock()
{
	const clock_scale_t clock = { /* 102MHz for my LED Serial Data Stuff */
		.pllm = 8,
		.plln = 408,
		.pllp = 4,
		.pllq = 2,
		.hpre = RCC_CFGR_HPRE_DIV_NONE,
		.ppre1 = RCC_CFGR_PPRE_DIV_4,
		.ppre2 = RCC_CFGR_PPRE_DIV_4,
		.power_save = 0,
		.flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE |
			FLASH_ACR_LATENCY_3WS,
		.apb1_frequency = 25500000,
		.apb2_frequency = 25500000,
	};
	// Set the System Clock to 102MHz!!
	rcc_clock_setup_hse_3v3(&clock);
}

static void setup_onboard_led(void)
{
	/* Enable GPIOD clock. */
	rcc_periph_clock_enable(RCC_GPIOD);

	/* Set GPIO12 (in GPIO port D) to 'output push-pull'. */
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
}

static void setup_timer(void)
{
	rcc_periph_clock_enable(RCC_TIM2);

	timer_reset(TIM2);

	/*
	 * Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Run timer at 1kHz.
	 *
	 * TIM2's base clock is APB1 * 2.
	 */
	uint32_t prescaler = ((2*rcc_apb1_frequency) / 1000);
	timer_set_prescaler(TIM2, prescaler);

	/* Force an update to load the prescaler */
	timer_generate_event(TIM2, TIM_EGR_UG);

	timer_enable_counter(TIM2);
}

void cylon(color *data, int len, int center);
void rainbowCycle(color *data, int len, int j);

int main(void)
{
	uint32_t i, j;

	setup_main_clock();
	setup_onboard_led();
	setup_timer();
	setup_spi();

	color led_data[N_LEDS];
	color scratch[N_LEDS];

	//union leds

	int x = 0;
	for(x=0; x < N_LEDS; x++)
	{
		led_data[x].r = 0;
		led_data[x].g = 0;
		led_data[x].b = 0;
	}

	int center = 0;
	int up = 1;
	j = 0;

	while (1) {
		uint32_t time = timer_get_counter(TIM2);

		// Blink the first LED green sometimes
		if((j % 100) == 0) {
			led_data[0].g = 255;
			gpio_set(GPIOD, GPIO12);
		}
		if((j % 100) == 2) {
			led_data[0].g = 0;
			gpio_clear(GPIOD, GPIO12);
		}

		// Make a cool effect plz!
		//shiftdecay(led_data, scratch, N_LEDS);

		//rainbowCycle(led_data, N_LEDS, j);
		cylon(led_data, N_LEDS, center);
		if (up) {
			center++;
			if (center >= N_LEDS) {
				up = 0;
			}
		} else {
			center--;
			if (center < 0) {
				up = 1;
			}
		}

		// Send the new data to the LED string
		update_string(led_data, N_LEDS);

		// Delay
		while (timer_get_counter(TIM2) <= (time+100));
		j++;
	}

	return 0;
}

color Wheel(int WheelPos);

void rainbowCycle(color *data, int len, int j) {
	int i;
	for(i=0; i< len; i++) {
		color c=Wheel(((i * 256 / len) + j) & 255);
		data[i] = c;
	}
}

color Wheel(int WheelPos) {
	static color c;

	if(WheelPos < 85) {
		c.r=WheelPos * 3;
		c.g=255 - WheelPos * 3;
		c.b=0;
	} else if(WheelPos < 170) {
		WheelPos -= 85;
		c.r=255 - WheelPos * 3;
		c.g=0;
		c.b=WheelPos * 3;
	} else {
		WheelPos -= 170;
		c.r=0;
		c.g=WheelPos * 3;
		c.b=255 - WheelPos * 3;
	}

	return c;
}

void cylon(color *data, int len, int center)
{
	int i;
	for (i = 0; i < len; i++) {
		if (i == center-2) {
			data[i].r = 32;
		} else if (i == center-1) {
			data[i].r = 64;
		} else if (i == center) {
			data[i].r = 128;
		} else if (i == center+1) {
			data[i].r = 64;
		} else if (i == center+2) {
			data[i].r = 32;
		} else {
			data[i].r = 0;
		}
		data[i].g = 0;
		data[i].b = 0;
	}
}

void subfloor(color *data, uint8_t d, uint16_t len)
{
	uint16_t i = 0;
	int16_t tmpr, tmpg, tmpb;
	for(i=0; i < len; i++)
	{
		tmpr = data[i].r - data[i].r/6;
		if(tmpr < 0) tmpr = 0;

		tmpg = data[i].g - data[i].g/6;
		if(tmpg < 0) tmpg = 0;

		tmpb = data[i].b - data[i].b/6;
		if(tmpb < 0) tmpb = 0;

		data[i].r = tmpr;
		data[i].g = tmpg;
		data[i].b = tmpb;
	}

}

void shiftdecay(color *data, color *buf, uint16_t len)
{
	uint16_t i = 0;

	// Shift & initial decay into buffer
	for(i = 1; i < len; i++)
	{
		buf[i].r = data[i-1].r;
		buf[i].g = data[i-1].g;
		buf[i].b = data[i-1].b;
	}

	// Add buf back in
	for(i = 1; i < len; i++)
	{
		data[i].r = buf[i].r;
		data[i].g = buf[i].g;
		data[i].b = buf[i].b;
	}
	data[0].r = data[len-1].r;
	data[0].g = data[len-1].g;
	data[0].b = data[len-1].b;

	// Decay (without underflowing)
	subfloor(data, 5, N_LEDS);
}
