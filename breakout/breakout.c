
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

#define PADDLE_SPRITE 10
#define BALL_SPRITE 10
#define SPRITE_STEP 4
#define PADDLE_Y 180
#define PADDLE_X_INIT 140

static uint8_t pulses;
static uint8_t paddle_x1;
static uint8_t paddle_x2;
static uint8_t ball_x1;
static uint8_t ball_x2;
static uint8_t ball_y1;
static uint8_t ball_y2;
static uint8_t ball_dx_p;
static uint8_t ball_dx_n;
static uint8_t ball_dy_p;
static uint8_t ball_dy_n;
static uint8_t ball_speed_x;
static uint8_t ball_speed_y;
static uint8_t start;

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

    uint8_t sprite_xl = SPRITE_XL_CALC(x1);
    uint8_t sprite_xh = SPRITE_XH_CALC(x1);

    sprite_hgr_to_buffer(sprite_xh, y1);
    sprite_xor(sprite1, 1, 1, sprite_xl);

    sprite_xl = SPRITE_XL_CALC(x2);
    col_delta += SPRITE_XH_CALC(x2) - sprite_xh;

    sprite_xor(sprite2, col_delta, row_delta, sprite_xl);
    sprite_buffer_to_hgr(sprite_xh, y1);
}


static void hbox(void)
{
    hires_hline(8, ROW_FIRST, 24, WHITE);
    hires_hline(8, ROW_FIRST + 1, 24, WHITE);
    hires_vline(8, 2, 191, 0x03);
    hires_vline(31, 2, 191, 0x60);
}

void blocks(void)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    for (i = 0; i < 20; i++)
    {
        j = 8 * i + 48;
        k = j - 2;
        sprite_update(0, j, 20, 13, j, 20);
        sprite_update(0, j, 28, 11, j, 28);

        sprite_update(0, k, 60, 14, k, 60);
        sprite_update(0, k, 68, 12, k, 68);
    }
}

void main(void)
{
    pointers_init();
    sprites_init();
    hires_clr();
    hbox();

    paddle_x1 = PADDLE_X_INIT;
    paddle_x2 = paddle_x1;

    ball_x1 = 110;
    ball_x2 = ball_x1;
    ball_y1 = 110;
    ball_y2 = ball_y1;
    ball_speed_x = 4;
    ball_speed_y = 0;
    ball_dx_p = ball_speed_x;
    ball_dx_n = 0;
    ball_dy_p = 0;
    ball_dy_n = 0;

    start = 0;

    sprite_update(0, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x2, PADDLE_Y);
    sprite_update(0, paddle_x1 - 4, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - 4, PADDLE_Y);
    sprite_update(0, paddle_x1 + 4, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + 4, PADDLE_Y);

    sprite_update(0, ball_x1, ball_y1, BALL_SPRITE, ball_x2, ball_y2);

    blocks();

    while(1)
    {

        if (paddle_x2 > paddle_x1)
        {
            sprite_update(PADDLE_SPRITE, paddle_x1 - 4, PADDLE_Y, 0, paddle_x1 - 4, PADDLE_Y);
            sprite_update(0, paddle_x2 + 4, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + 4, PADDLE_Y);
        }
        if (paddle_x2 < paddle_x1)
        {
            sprite_update(PADDLE_SPRITE, paddle_x1 + 4, PADDLE_Y, 0, paddle_x1 + 4, PADDLE_Y);
            sprite_update(0, paddle_x2 - 4, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - 4, PADDLE_Y);
        }
        if (paddle_x2 == paddle_x1)
        {
            sprite_update(PADDLE_SPRITE, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x1, PADDLE_Y);
            sprite_update(PADDLE_SPRITE, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x1, PADDLE_Y);
        }


        paddle_x1 = paddle_x2;

        sprite_update(BALL_SPRITE, ball_x1, ball_y1, BALL_SPRITE, ball_x2, ball_y2);

        ball_x1 = ball_x2;
        ball_y1 = ball_y2;

        if (ball_x2 > 200)
        {
            ball_dx_p = 0;
            ball_dx_n = ball_speed_x;
            pulses = 5;
        }

        if (ball_x2 < 50)
        {
            ball_dx_p = ball_speed_x;
            ball_dx_n = 0;
            pulses = 5;
        }

        if (ball_y2 > 200)
        {
            ball_dy_p = 0;
            ball_dy_n = ball_speed_y;
            pulses = 5;
        }

        if (ball_y2 < 50)
        {
            ball_dy_p = ball_speed_y;
            ball_dy_n = 0;
            pulses = 5;
        }

        ball_x2 += ball_dx_p - ball_dx_n;
        ball_y2 += ball_dy_p - ball_dy_n;

        joystick_run();

        if (joystick_left_get())
        {
            if (paddle_x2 > 50)
            {
                paddle_x2 -= SPRITE_STEP;
            }
        }

        if (joystick_right_get())
        {
            if (paddle_x2 < 200)
            {
                paddle_x2 += SPRITE_STEP;
            }
        }

        if (joystick_fire_get())
        {
            if (start == 0)
            {
                start = 1;
                ball_speed_y = 4;
                ball_dy_p = ball_speed_y;
            }
        }

        if (pulses > 0)
        {
            sound(pulses);
            pulses--;
        }

        // TEST_PIN_TOGGLE; // adds 2.5us
        // TEST_PIN_TOGGLE; // adds 2.5us
    }
}


// 1.023 MHz / 30 Hz = 34100 cycles
// 34100 / 255 = 133 cycles per count
