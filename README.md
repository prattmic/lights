# STM32F4 powered Christmas lights

## About

This project uses the SPI peripheral in a STM32F4 to communicate with WS2811
RGB LEDs. The PLL on the STM32F4 is tuned to 408MHz, and a total divisor of 32
is applied before going to SPI1. This makes it so that 16bits from the SPI
output matchup reasonably well with the expected bit period of 2.5us.

The `update_string` function outputs data to the WS2811 string by transmitting
different numbers of sequential ones in order to generate the duty cycles that
the WS2811 expects for a 'one' vs 'zero' bit.

This project is intended to run on the STM32F4(07) Discovery board. The SPI
output is on PA7.

## Clock System Setup

8MHz Xtal \-\-> Div/8 \-\-> PLL x 408 \-\-> Div/4 \-\-> SYSCLK \-\-> PPRE1/2
\-\-> 51MHz for SPI1 \(with DIV/4\ inside the SPI peripheral)

This gives us 12.75MHz for the SPI clock. Using 16 SPI bits per WS2811 bit
period we get close to the WS2811's desired bit rate at 1.25us per bit period.

## Other Parts Supported

This code will work with other parts that use the same communication protocol
such as, TM1809, and WS2812B aka the neopixel.

## Simulator

There is also a simulator for testing LED effects without the hardware. It
consists of two parts:

### Microcontroller simulator

This C program runs the same effect functions as the microcontroller, but
instead of updating an actual LED string, it writes updates to stdout in the
form:

```
R1,G1,B1,R2,G2,B2,...,RN,BN,GN
```
Build it in the `lights` directory:

```
$ cd lights
$ make simulator
```

### LED visualizer

This Python program reads outputs from the microcontroller simulator and
displays them in a simulated LED string.

Install the dependencies, then connect it to the simulator:

```
$ cd gui
# Create a virtualenv for the dependencies
$ virtualenv venv
# Enter the virtualenv
$ source venv/bin/activate
# Install the dependencies
$ pip install -r requirements.txt
# Go!
$ ../lights/simulator | python visualize.py
```
