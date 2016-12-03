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
#include <stdlib.h>
#include <math.h>

#define N_LEDS 50

// Single pixel RGB data structure. Make an array out of this to store RGB data for a string.
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color;


void setup_spi(void);
void setup_peripheral_clocks(void);
void setup_main_clock(void);

void update_string(color *data, uint16_t len);
void shiftdecay(color *data, color *buf, uint16_t len);

static void setup_onboard_led(void)
{
	/* Enable GPIOD clock. */
	rcc_periph_clock_enable(RCC_GPIOD);

	/* Set GPIO12 (in GPIO port D) to 'output push-pull'. */
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
}

void setup_main_clock()
{
	const clock_scale_t clock = { /* 102MHz for my LED Serial Data Stuff */
		.pllm = 8,
		.plln = 408,
		.pllp = 4,
		.pllq = 2,
		.hpre = RCC_CFGR_HPRE_DIV_NONE,
		.ppre1 = RCC_CFGR_PPRE_DIV_2,
		.ppre2 = RCC_CFGR_PPRE_DIV_4,
		.power_save = 0,
		.flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE |
			FLASH_ACR_LATENCY_3WS,
		.apb1_frequency = 25500000,
		.apb2_frequency = 51000000,
	};
	// Set the System Clock to 102MHz!!
	rcc_clock_setup_hse_3v3(&clock);
}

void setup_spi()
{
    /* We are using GPIOA pins. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_SPI1);

	/*
	 * SPI1 uses the following pins:
	 *
	 * PA5: serial clock
	 * PA6: master in/slave out
	 * PA7: master out/slave in
	 * PA4: master slaveselect out
	 *
	 * We only enable PA7, because all we care about is the output.
	 */

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO7);

	spi_disable_crc(SPI1);

	// Set the divider to 16 so that we get 6.4MHz for our LED communication!!
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_4,
			/* high or low for the peripheral device */
			SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
			/* CPHA: Clock phase: read on rising edge of clock */
			SPI_CR1_CPHA_CLK_TRANSITION_2,
			/* DFF: Date frame format (8 or 16 bit) */
			SPI_CR1_DFF_16BIT,
			/* Most or Least Sig Bit First */
			SPI_CR1_MSBFIRST);

	spi_enable(SPI1);
}

void cylon(color *data, int len, int center);
void rainbowCycle(color *data, int len, int j);

int main(void)
{
	uint32_t i, j;

	setup_main_clock();
	setup_onboard_led();
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
		//gpio_toggle(GPIOD, GPIO12);	/* LED on/off */

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

		rainbowCycle(led_data, N_LEDS, j);
		//if (up) {
		//	center++;
		//	if (center >= N_LEDS) {
		//		up = 0;
		//	}
		//} else {
		//	center--;
		//	if (center < 0) {
		//		up = 1;
		//	}
		//}

		// Send the new data to the LED string
		update_string(led_data, N_LEDS);

		// Delay
		for (i = 0; i < 400000; i++) {	/* Wait a bit. */
			__asm__("nop");
		}
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

/* turn bits into pulses of the correct ratios for the WS2811 by making *
 * bytes with the correct number of ones & zeros in the right order.    */
void update_string(color *data, uint16_t len)
{
	uint8_t *bytearray = (uint8_t *) data;

	uint16_t i = 0;
	int16_t j = 0;
	uint8_t tmp = 0;

	len = len * 3;
	for(i=0; i < len; i++)
	{
		tmp = bytearray[i];
		for(j = 7; j >= 0; j--)
		{
			if (tmp & (0x01 << j))
			{
				// generate the sequence to represent a 'one' to the WS2811.
				spi_send(SPI1, 0xFF00);
					     //0b1111 1111 0000 0000
			}
			else
			{
				// generate the sequence to represent a 'zero'.
				spi_send(SPI1, 0xE000);
					     //0b1110 0000 0000 0000
			}
		}
	}
}
