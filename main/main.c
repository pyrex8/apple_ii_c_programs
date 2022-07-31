
// Note all timing is based on the slower emulated apple IIe that is taking
// approx 1.5us per cycle.

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../lib/test_pin.h"
#include "../lib/zero_page.h"
#include "../lib/hires.h"
#include "../lib/sprites.h"
#include "../lib/joystick.h"
#include "../lib/sound.h"


static const uint8_t lklo[256] = {HIRES_MEMORY_LOW_BYTE};
static const uint8_t lkhi[256] = {HIRES_MEMORY_HIGH_BYTE};
const uint8_t div7[256] = {DIV7};
const uint8_t mod7[256] = {MOD7};

#define SPRITE_XH_CALC(x) (div7[x])
#define SPRITE_XL_CALC(x) (mod7[x])

#define SPRITE_PLAYER 4

static uint8_t pulses;
static uint8_t sprite_x1;
static uint8_t sprite_y1;
static uint8_t sprite_x2;
static uint8_t sprite_y2;
static uint8_t sprite_xl;
static uint8_t sprite_xh;
static uint8_t sprite_no_jump;

static void pointers_init(void)
{
    LKLOL_P = (uint8_t)lklo;
    LKLOH_P = (uint8_t)(((uint16_t)lklo)>> 8);
    LKHIL_P = (uint8_t)lkhi;
    LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> 8);
}

void sprite_update(uint8_t sprite1, uint8_t x1, uint8_t y1, uint8_t sprite2, uint8_t x2, uint8_t y2)
{
    // 9200us
    uint8_t col_delta = 1;
    uint8_t row_delta = 1 + y2 - y1;

    sprite_xl = SPRITE_XL_CALC(x1);
    sprite_xh = SPRITE_XH_CALC(x1);

    sprite_hgr_to_buffer(sprite_xh, y1);
    sprite_xor(sprite1, 1, 1, sprite_xl);

    sprite_xl = SPRITE_XL_CALC(x2);
    col_delta += SPRITE_XH_CALC(x2) - sprite_xh;

    sprite_xor(sprite2, col_delta, row_delta, sprite_xl);
    sprite_buffer_to_hgr(sprite_xh, y1);
}


static void hbox(void)
{
    hires_hline(COLUMN_FIRST, ROW_FIRST, COLUMNS, WHITE);
    hires_hline(COLUMN_FIRST, ROW_LAST, COLUMNS, WHITE);
    hires_vline(COLUMN_FIRST, 0, 192, 0x03);
    hires_vline(COLUMN_LAST, 0, 192, 0x60);
}

void delay(void)
{
    uint8_t i = 0;
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
}

void main(void)
{
    pointers_init();
    sprites_init();
    hires_clr();
    hbox();

    sprite_no_jump = 1;
    sprite_x1 = 140;
    sprite_y1 = 100;
    sprite_x2 = sprite_x1;
    sprite_y2 = sprite_y1;
    sprite_update(0, sprite_x1, sprite_y1, SPRITE_PLAYER, sprite_x2, sprite_y2);

    sprite_update(0, 20, 150, 1, 20, 150);
    sprite_update(0, 40, 150, 2, 40, 150);
    sprite_update(0, 60, 150, 3, 60, 150);
    sprite_update(0, 80, 150, 4, 80, 150);
    sprite_update(0, 100, 150, 5, 100, 150);
    sprite_update(0, 120, 150, 6, 120, 150);
    sprite_update(0, 140, 150, 7, 140, 150);
    sprite_update(0, 160, 150, 8, 160, 150);
    sprite_update(0, 180, 150, 9, 180, 150);
    sprite_update(0, 200, 150, 10, 200, 150);

    while(1)
    {


        if (sprite_x2 > 240)
        {
            sprite_update(SPRITE_PLAYER, sprite_x1, sprite_y1, 0, sprite_x2, sprite_y2);
            sprite_x2 = 10;
            sprite_x1 = sprite_x2;
            sprite_y1 = sprite_y2;
            sprite_update(0, sprite_x1, sprite_y1, SPRITE_PLAYER, sprite_x2, sprite_y2);
            sprite_no_jump = 0;
        }

        if (sprite_no_jump)
        {
            sprite_update(SPRITE_PLAYER, sprite_x1, sprite_y1, SPRITE_PLAYER, sprite_x2, sprite_y2);

            sprite_x1 = sprite_x2;
            sprite_y1 = sprite_y2;
        }
        sprite_no_jump = 1;

        joystick_run();


        if (joystick_up_get())
        {
            sprite_y2 -= 2;
        }

        if (joystick_down_get())
        {
            sprite_y2 += 2;
        }

        if (joystick_left_get())
        {
            sprite_x2 -= 2;
        }

        if (joystick_right_get())
        {
            sprite_x2 += 2;
        }

        if (joystick_fire_get())
        {
            pulses = 5;
        }

        if (pulses > 0)
        {
            sound(pulses);
            pulses--;
        }


        delay();

        // TEST_PIN_TOGGLE; // adds 2.5us
        // hires_hline(5, 5, 40, WHITE);
        // TEST_PIN_TOGGLE; // adds 2.5us
        // hires_vline(10, 0, 192, 0x03);

    }
}
