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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/timer.h>

#include "effects.h"
#include "led_string.h"

#define NUM_LEDS 50

static void setup_main_clock(void)
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

int main(void)
{
	setup_main_clock();
	setup_onboard_led();
	setup_timer();
	setup_spi();

	struct color led_data[NUM_LEDS] = {0};
	effect_func effect = random_effect();

	/* The loop runs every 10ms. */
	for (uint32_t i = 0; ; i++) {
		uint32_t time = timer_get_counter(TIM2);

		/* New effect every 10s */
		if ((i % 1000) == 0) {
			memset(led_data, 0, sizeof(led_data));
			effect = random_effect();
		}

		/* Blink the green LED once per second. */
		if((i % 100) == 0) {
			gpio_set(GPIOD, GPIO12);
		}
		if((i % 100) == 2) {
			gpio_clear(GPIOD, GPIO12);
		}

		/* Step the effect */
		effect(i, led_data, NUM_LEDS);

		/* Send the new data to the LED string. */
		update_string(led_data, NUM_LEDS);

		/* Delay */
		while (timer_get_counter(TIM2) <= (time+10));
	}

	return 0;
}
