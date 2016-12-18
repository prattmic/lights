import sys, pygame
pygame.init()

size = width, height = 500, 10
black = 0, 0, 0

screen = pygame.display.set_mode(size)

def redraw(leds):
    screen.fill(black)

    for i, led in enumerate(leds):
        pygame.draw.rect(screen, led, (10*i, 0, 10, 10))

while 1:
    for event in pygame.event.get():
        if event.type == pygame.QUIT: sys.exit()

    leds = [(255, 255, 255), (255, 0, 0), (0, 255, 0), (0, 0, 255)]

    redraw(leds)
    pygame.display.flip()
