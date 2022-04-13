#!/usr/bin/env python3
"""
apple iie 7 x 8 pixel HIRES sprite editor
"""

import os
import time
import pygame
import pygame.sndarray
import pygame.surfarray
import numpy
import sys

PIXELS_X = 280
PIXELS_Y = 192
SCREEN_SCALE = 4
SCREEN_X = PIXELS_X * SCREEN_SCALE
SCREEN_Y = PIXELS_Y * SCREEN_SCALE
SCREEN_X_TOTAL = SCREEN_X
SCREEN_Y_TOTAL = SCREEN_Y

LINE_OFFSET = 4 * SCREEN_SCALE + 3
PIXEL_GRID = 6
PIXEL_SIZE = 4

BLACK = (0, 0, 0)
GREEN = (66, 230, 6)
PURPLE = (186, 23, 245)
ORANGE = (228, 83, 4)
BLUE = (25, 171, 249)
WHITE = (255, 255, 255)
GREY = (128, 128, 128)

HCOLOR = [BLACK, PURPLE, GREEN, GREEN, PURPLE, BLUE, ORANGE, ORANGE, BLUE, WHITE]

sprite =[ [0] * 8 for _ in range(8)]
sprite[2][2] = 1

joystick_present = 0
button_0 = 0
paddle_0 = 0
paddle_1 = 0

cursor_x = 10
cursor_y = 10

def pixel(x , y, color):
    pygame.draw.rect(screen, color, (x * SCREEN_SCALE, y * SCREEN_SCALE , SCREEN_SCALE, SCREEN_SCALE))

def pixel_4x(x , y, color):
    pygame.draw.rect(screen, color, (x * SCREEN_SCALE * PIXEL_GRID, y * SCREEN_SCALE * PIXEL_GRID , SCREEN_SCALE * 4, SCREEN_SCALE * 4))

def cursor_4x(x , y, color):
    x1 = x * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    y1 = y * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    x2 = (x + 1) * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    y2 = (y + 1) * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    pygame.draw.line(screen, color, (x1, y1), (x1, y2), 1)
    pygame.draw.line(screen, color, (x2, y1), (x2, y2), 1)
    pygame.draw.line(screen, color, (x1, y1), (x2, y1), 1)
    pygame.draw.line(screen, color, (x1, y2), (x2, y2), 1)

def line_4x(x1 , y1, x2, y2, color):
    pygame.draw.line(screen, color, (x1 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET, y1 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET), (x2 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET, y2 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET), SCREEN_SCALE)

pygame.init()
pygame.display.set_caption('pyrex8')

screen = pygame.display.set_mode([SCREEN_X_TOTAL, SCREEN_Y_TOTAL])
clock = pygame.time.Clock()

running = True

while running:

    if joystick_present:
        button_0 = int(joystick.get_button(0))
        paddle_0 = int((joystick.get_axis(0) + 1) * 127)
        paddle_1 = int((joystick.get_axis(1) + 1) * 127)
    else:
        if pygame.joystick.get_count() > 0:
            joystick = pygame.joystick.Joystick(0)
            joystick.init()
            joystick_present = 1
        button_0 = 0
        paddle_0 = 128
        paddle_1 = 128

    keycode = 0

    for event in pygame.event.get():

        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_F12:
                running = False

            if event.key == pygame.K_F1:
                keycode = 128

            if event.key == pygame.K_F2:
                keycode = 129

            if event.key == pygame.K_UP:
                keycode = 0x0B
            if event.key == pygame.K_DOWN:
                keycode = 0x0A
            if event.key == pygame.K_LEFT:
                keycode = 0x08
            if event.key == pygame.K_RIGHT:
                keycode = 0x15
            if event.key == pygame.K_SPACE:
                keycode = pygame.K_SPACE

    pygame.draw.rect(screen, BLACK, (0, 0, SCREEN_X_TOTAL, SCREEN_Y_TOTAL))

    # HCOLOR
    # BLACK, PURPLE, GREEN, GREEN, PURPLE, BLUE, ORANGE, ORANGE, BLUE, WHITE
    if paddle_0 > 200 or keycode == 0x15:
        cursor_x += 1
    if paddle_0 < 55  or keycode == 0x08:
        cursor_x -= 1
    if paddle_1 > 200 or keycode == 0x0A:
        cursor_y += 1
    if paddle_1 < 55  or keycode == 0x0B:
        cursor_y -= 1

    if button_0 or keycode == pygame.K_SPACE:
        sprite[cursor_x - 10][cursor_y - 10] ^= 1

    if cursor_x < 10:
        cursor_x = 17
    if cursor_x > 17:
        cursor_x = 10
    if cursor_y < 10:
        cursor_y = 17
    if cursor_y > 17:
        cursor_y = 10



    for y in range(8):
        for x in range(8):
            if sprite[x][y] == 1:
                pixel_4x(10 + x, 10 + y, WHITE)

    for i in range(9):
        line_4x(9, 9 + i, 17, 9 + i, GREY)
        line_4x(9 + i, 9, 9 + i, 17, GREY)

    cursor_4x(cursor_x - 1, cursor_y - 1, WHITE)

    pygame.display.flip()

    clock.tick(10)

pygame.quit()
