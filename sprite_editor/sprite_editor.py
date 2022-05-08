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

SPRITE_X = 8
SPRITE_Y = 8

CURSOR_OFFSET_X = 9
CURSOR_OFFSET_Y = 9

BLACK = (0, 0, 0)
GREEN = (66, 230, 6)
PURPLE = (186, 23, 245)
ORANGE = (228, 83, 4)
BLUE = (25, 171, 249)
WHITE = (255, 255, 255)
GREY = (128, 128, 128)

HCOLOR = [BLACK, PURPLE, GREEN, GREEN, PURPLE, BLUE, ORANGE, ORANGE, BLUE, WHITE]

sprite = [[0] * SPRITE_X for _ in range(SPRITE_Y)]
line_colors = [0] * SPRITE_Y
sprite_data = [[0] * 16 for _ in range(SPRITE_Y)]
sprite_color = [[0] * SPRITE_X for _ in range(SPRITE_Y)]

sprite[0][0] = 1
sprite[7][7] = 1
sprite_color[0][0] = 1
sprite_color[7][7] = 2

cursor_x = 0
cursor_y = 0

def running_test(running, keycode):
    running_rtn = running
    if keycode == pygame.K_F10:
        running_rtn = False
    return running_rtn

def screen_clear():
    pygame.draw.rect(screen, BLACK, (0, 0, SCREEN_X_TOTAL, SCREEN_Y_TOTAL))

def pixel(x , y, color):
    px = x * SCREEN_SCALE
    py =  y * SCREEN_SCALE
    pygame.draw.rect(screen, color, (px , py, SCREEN_SCALE, SCREEN_SCALE))

def pixel_4x(x , y, color):
    x4 = x * SCREEN_SCALE * PIXEL_GRID
    y4 = y * SCREEN_SCALE * PIXEL_GRID
    pygame.draw.rect(screen, color, (x4, y4, SCREEN_SCALE * 4, SCREEN_SCALE * 4))

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
    lx1 = x1 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    ly1 = y1 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    lx2 = x2 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    ly2 = y2 * SCREEN_SCALE * PIXEL_GRID + LINE_OFFSET
    pygame.draw.line(screen, color, (lx1, ly1), (lx2, ly2), SCREEN_SCALE)

def update_cursor(cursor_x, cursor_y, keycode):

    if keycode == pygame.K_RIGHT:
        cursor_x += 1
    if keycode == pygame.K_LEFT:
        cursor_x -= 1
    if keycode == pygame.K_DOWN:
        cursor_y += 1
    if keycode == pygame.K_UP:
        cursor_y -= 1

    if cursor_x < 0:
        cursor_x = SPRITE_X - 1
    if cursor_x > SPRITE_X - 1:
        cursor_x = 0
    if cursor_y < 0:
        cursor_y = SPRITE_Y - 1
    if cursor_y > SPRITE_Y - 1:
        cursor_y = 0

    return cursor_x, cursor_y

def pixel_update(pixel, keycode):
    pixel_rtn = pixel
    if keycode == pygame.K_SPACE:
        pixel_rtn ^= 1
    return pixel_rtn

def cursor_grid_draw(line_colors, sprite):

    for y in range(8):
        if line_colors[y] == 1:
            pixel_4x(8, 10 + y, WHITE)

        for x in range(8):
            if sprite[y][x] == 1:
                pixel_4x(10 + x, 10 + y, WHITE)
                pixel(180 + x, 60 + y, WHITE)

    line_4x(7, 9, 7, 17, GREY)
    line_4x(8, 9, 8, 17, GREY)

    for i in range(9):
        line_4x(9, 9 + i, 17, 9 + i, GREY)
        line_4x(9 + i, 9, 9 + i, 17, GREY)

        line_4x(7, 9 + i, 8, 9 + i, GREY)

def sprite_color_draw(loc_x, loc_y):

    pixel_present = 0
    address_odd = (int(loc_x / 8) & 1) << 1
    color_offset = 0
    pixel_left = 0
    pixel_right = 0

    for y in range(SPRITE_Y):
        color_offset = line_colors[y] << 2
        pixel_left = 0
        pixel_present = 0
        pixel_right = sprite[y][0]
        for x in range(SPRITE_X):
            pixel_left = pixel_present
            pixel_present = pixel_right
            if x < SPRITE_X - 1:
                pixel_right = sprite[y][x + 1]
            else:
                pixel_right = 0

            if pixel_present:
                if pixel_left or pixel_right:
                    sprite_color[y][x] = 9
                else:
                    sprite_color[y][x] = color_offset + address_odd + (x & 1) + 1
            else:
                if pixel_left and pixel_right:
                    sprite_color[y][x] = color_offset + address_odd - (x & 1) + 2
                else:
                    sprite_color[y][x] = 0

            pixel(x + loc_x, y + loc_y, HCOLOR[sprite_color[y][x]])

def sprite_data_print(sprite_data):
    if keycode == pygame.K_F1:
        print("sprite data:")
        for i in range(8):
            print(", ".join("0x{:02X}".format(num) for num in sprite_data[i]) + ",")


pygame.init()
pygame.display.set_caption('pyrex8')

screen = pygame.display.set_mode([SCREEN_X_TOTAL, SCREEN_Y_TOTAL])
clock = pygame.time.Clock()

running = True

while running:

    keycode = 0

    for event in pygame.event.get():

        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_F10:
                keycode = pygame.K_F10

            if event.key == pygame.K_F1:
                keycode = pygame.K_F1

            if event.key == pygame.K_1:
                line_colors[cursor_y] ^= 1

            if event.key == pygame.K_UP:
                keycode = pygame.K_UP

            if event.key == pygame.K_DOWN:
                keycode = pygame.K_DOWN

            if event.key == pygame.K_LEFT:
                keycode = pygame.K_LEFT

            if event.key == pygame.K_RIGHT:
                keycode = pygame.K_RIGHT

            if event.key == pygame.K_SPACE:
                keycode = pygame.K_SPACE


    running  = running_test(running, keycode)
    screen_clear()
    cursor_x, cursor_y = update_cursor(cursor_x, cursor_y, keycode)
    sprite[cursor_y][cursor_x] = pixel_update(sprite[cursor_y][cursor_x], keycode)
    cursor_grid_draw(line_colors, sprite)
    cursor_4x(cursor_x + CURSOR_OFFSET_X, cursor_y + CURSOR_OFFSET_Y, WHITE)

    sprite_data_print(sprite_data)

    for col in range(8):
        for row in range(8):
            sprite_data[row][col * 2] = 0
            sprite_data[row][col * 2 + 1] = 0

    for col in range(8):
        for row in range(7):
            # 8 pixel bits shift on each y increment
            if row == 0:
                for pix in range(7):
                    sprite_data[row][col * 2] |= sprite[col][pix] << pix
                sprite_data[row][col * 2 + 1] |= sprite[col][7]
            # shift bits to the left if row > 0 fro previous row
            else:
                sprite_data[row][col * 2] = (sprite_data[row - 1][col * 2] & 0x7F) << 1
                sprite_data[row][col * 2 + 1] = (sprite_data[row - 1][col * 2 + 1] & 0x7F) << 1
                # move MSBit over to other byte
                if sprite_data[row][col * 2] & 0x80:
                    sprite_data[row][col * 2] &= 0x7F
                    sprite_data[row][col * 2 + 1] |= 0x01
            # color bit last
            sprite_data[row][col * 2] |= line_colors[col] << 7
            sprite_data[row][col * 2 + 1] |= line_colors[col] << 7

    sprite_color_draw(160, 100)
    sprite_color_draw(184, 100)

    pygame.display.flip()

    clock.tick(10)

pygame.quit()
