import fileinput
import pygame
import sys

pygame.init()

size = width, height = 1000, 50
black = 0, 0, 0

screen = pygame.display.set_mode(size)

def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in range(0, len(l), n):
        yield l[i:i + n]

def redraw(leds):
    screen.fill(black)

    for i, led in enumerate(leds):
        led = tuple([int(c) for c in led])
        pygame.draw.rect(screen, led, (20*i, 0, 20, 50))

for line in fileinput.input():
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit()

    channels = line.split(',')
    leds = chunks(channels, 3)

    redraw(leds)
    pygame.display.flip()
