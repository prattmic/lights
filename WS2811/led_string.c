#include "led_string.h"

#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

void setup_spi(void)
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

/* turn bits into pulses of the correct ratios for the WS2811 by making *
 * bytes with the correct number of ones & zeros in the right order.    */
void update_string(struct color *data, uint16_t len)
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
