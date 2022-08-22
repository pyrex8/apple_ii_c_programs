
// Note all timing is based on the slower emulated apple IIe that is taking
// approx 1.5us per cycle.

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../lib/test_pin.h"
#include "../lib/delay.h"
#include "../lib/zero_page.h"
#include "../lib/hires.h"
#include "../lib/sprite.h"
#include "../lib/joystick.h"
#include "../lib/sound.h"
#include "../lib/digit.h"

static const uint8_t lklo[] = {HIRES_MEMORY_LOW_BYTE};
static const uint8_t lkhi[] = {HIRES_MEMORY_HIGH_BYTE};
const uint8_t div7[] = {DIV7};
const uint8_t mod7[] = {MOD7};

#define SPRITE_XH_CALC(x) (div7[x])
#define SPRITE_XL_CALC(x) (mod7[x])

#define BYTE_HIGH_BITS 8
#define SOUND_BOUNCE 5
#define SOUND_END 30

#define PADDLE_SPRITE 10
#define SPRITE_STEP 6
#define SPRITE_STEP2 (2 * SPRITE_STEP)
#define PADDLE_X_MIN 58
#define PADDLE_X_MAX 188
#define PADDLE_Y 90
#define PADDLE_X_INIT PADDLE_X_MIN + (SPRITE_STEP * 10)
#define PADDLE_WIDTH 18
#define PADDLE_CENTER 4
#define PADDLE_COLLISION_MIN (PADDLE_Y - PADDLE_CENTER)
#define PADDLE_COLLISION_MAX PADDLE_Y

#define SCORE_Y 0

enum direction
{
    DIRECTION_NONE = 0,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
};


static uint8_t pulses;
static uint8_t paddle_x1;
static uint8_t paddle_x2;
static uint8_t start;
static uint8_t end;
static uint8_t score;
static uint8_t high_score;
static uint8_t score_ones;
static uint8_t score_tens;
static uint8_t score_hundreds;
static uint8_t high_ones;
static uint8_t high_tens;
static uint8_t high_hundreds;
static uint8_t ship_direction;
static uint8_t ship_direction_new;

static void pointers_init(void)
{
    LKLOL_P = (uint8_t)lklo;
    LKLOH_P = (uint8_t)(((uint16_t)lklo)>> BYTE_HIGH_BITS);
    LKHIL_P = (uint8_t)lkhi;
    LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> BYTE_HIGH_BITS);
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
    hires_hline(6, 0, 28, WHITE);
    hires_hline(6, 1, 28, WHITE);
    hires_hline(6, 190, 28, WHITE);
    hires_hline(6, 191, 28, WHITE);
    hires_vline(6, 2, 188, 0x03);
    hires_vline(33, 2, 188, 0x60);
}

void game_init(void)
{
    paddle_x1 = PADDLE_X_INIT;
    paddle_x2 = paddle_x1;

    start = 0;
    end = 0;
    pulses = 0;

    ship_direction = DIRECTION_NONE;
    ship_direction_new = DIRECTION_UP;

    if (score > high_score)
    {
        high_score = score;
        high_ones = score_ones;
        high_tens = score_tens;
        high_hundreds = score_hundreds;
    }

    score = 0;
    score_ones = 0;
    score_tens = 0;
    score_hundreds = 0;
}

void score_draw(void)
{
    digit_set(2, SCORE_Y, score_hundreds);
    digit_set(3, SCORE_Y, score_tens);
    digit_set(4, SCORE_Y, score_ones);
}

void high_score_draw(void)
{
    digit_set(35, SCORE_Y, high_hundreds);
    digit_set(36, SCORE_Y, high_tens);
    digit_set(37, SCORE_Y, high_ones);
}

void ship_draw(void)
{
    sprite_update(0, paddle_x1, PADDLE_Y, 15, paddle_x2, PADDLE_Y);
}

void ship_up_draw(void)
{
    sprite_update(0, paddle_x1 + 2, PADDLE_Y - 2, 5, paddle_x2 + 2, PADDLE_Y - 2);
    sprite_update(0, paddle_x1 + 2, PADDLE_Y - 4, 5, paddle_x2 + 2, PADDLE_Y - 4);
    sprite_update(0, paddle_x1 + SPRITE_STEP, PADDLE_Y + 4, 5, paddle_x2 + SPRITE_STEP, PADDLE_Y + 4);
    sprite_update(0, paddle_x1 - 2, PADDLE_Y + 4, 5, paddle_x2 - 2, PADDLE_Y + 4);
}

void ship_up_erase(void)
{
    sprite_update(5, paddle_x1 + 2, PADDLE_Y - 2, 0, paddle_x2 + 2, PADDLE_Y - 2);
    sprite_update(5, paddle_x1 + 2, PADDLE_Y - 4, 0, paddle_x2 + 2, PADDLE_Y - 4);
    sprite_update(5, paddle_x1 + SPRITE_STEP, PADDLE_Y + 4, 0, paddle_x2 + SPRITE_STEP, PADDLE_Y + 4);
    sprite_update(5, paddle_x1 - 2, PADDLE_Y + 4, 0, paddle_x2 - 2, PADDLE_Y + 4);
}

void ship_down_draw(void)
{
    sprite_update(0, paddle_x1 + 2, PADDLE_Y + 6, 5, paddle_x2 + 2, PADDLE_Y + 6);
    sprite_update(0, paddle_x1 + 2, PADDLE_Y + 8, 5, paddle_x2 + 2, PADDLE_Y + 8);
    sprite_update(0, paddle_x1 + SPRITE_STEP, PADDLE_Y, 5, paddle_x2 + SPRITE_STEP, PADDLE_Y);
    sprite_update(0, paddle_x1 - 2, PADDLE_Y, 5, paddle_x2 - 2, PADDLE_Y);
}

void ship_down_erase(void)
{
    sprite_update(5, paddle_x1 + 2, PADDLE_Y + 6, 0, paddle_x2 + 2, PADDLE_Y + 6);
    sprite_update(5, paddle_x1 + 2, PADDLE_Y + 8, 0, paddle_x2 + 2, PADDLE_Y + 8);
    sprite_update(5, paddle_x1 + SPRITE_STEP, PADDLE_Y, 0, paddle_x2 + SPRITE_STEP, PADDLE_Y);
    sprite_update(5, paddle_x1 - 2, PADDLE_Y, 0, paddle_x2 - 2, PADDLE_Y);
}

void ship_update(void)
{
    if (ship_direction_new == ship_direction)
    {
        return;
    }

    switch (ship_direction)
    {
        case DIRECTION_UP:
            ship_up_erase();
            break;
        case DIRECTION_DOWN:
            ship_down_erase();
            break;
        case DIRECTION_LEFT:
            ship_up_erase();
            break;
        case DIRECTION_RIGHT:
            ship_up_erase();
            break;
    }

    switch (ship_direction_new)
    {
        case DIRECTION_UP:
            ship_up_draw();
            break;
        case DIRECTION_DOWN:
            ship_down_draw();
            break;
        case DIRECTION_LEFT:
            ship_up_draw();
            break;
        case DIRECTION_RIGHT:
            ship_up_draw();
            break;
    }

    ship_direction = ship_direction_new;
}

void score_increase(void)
{
    score++;
    if (score_ones > 8)
    {
        score_ones = 0;
        if (score_tens > 8)
        {
            score_tens = 0;
            score_hundreds++;

        }
        else
        {
            score_tens++;
        }
    }
    else
    {
        score_ones++;
    }
    score_draw();
}

void delay(void)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        delay_100us();
    }
}

void main(void)
{
    high_score = 0;

    score_ones = 0;
    score_tens = 0;
    score_hundreds = 0;
    high_ones = 0;
    high_tens = 0;
    high_hundreds = 0;

    pointers_init();
    sprite_init();
    digit_init();

    game_init();
    hires_clr();
    hbox();

    ship_draw();
    ship_update();
    score_draw();
    high_score_draw();

    while(1)
    {
         paddle_x1 = paddle_x2;

        joystick_run();

        if (joystick_up_get())
        {
            ship_direction_new = DIRECTION_UP;
        }

        if (joystick_down_get())
        {
            ship_direction_new = DIRECTION_DOWN;
        }

        if (joystick_left_get())
        {
            ship_direction_new = DIRECTION_LEFT;
        }

        if (joystick_right_get())
        {
            ship_direction_new = DIRECTION_RIGHT;
        }

        ship_update();

        if (joystick_fire_get())
        {
            pulses = SOUND_BOUNCE;
            if (start == 0)
            {
                start = 1;
            }
            if (end == 1)
            {
                game_init();
                hires_clr();
                hbox();
                ship_draw();
                ship_up_draw();
                score_draw();
                high_score_draw();
            }
        }

        if (pulses > 0)
        {
            sound(pulses);
            pulses--;
        }

        delay();
    }
}
