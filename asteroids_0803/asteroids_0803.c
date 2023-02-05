
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

#define SHIP_X 120
#define SHIP_Y 90
#define SPRITE_ON 5
#define SPRITE_OFF 0

#define MISSILE_SPRITE 5
#define MISSILE_SPEED 4
#define MISSILE_X_INIT 120
#define MISSILE_Y_INIT 90
#define MISSILE_OFFSET 4

#define ASTEROID_SPRITE 5
#define ASTEROID_SPEED 1
#define ASTEROID_X_INIT 100
#define ASTEROID_Y_INIT 50

#define X_MIN 38
#define X_MAX 210
#define Y_MIN 4
#define Y_MAX 176

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

static uint8_t missile_ready;
static uint8_t missile_x1;
static uint8_t missile_y1;
static uint8_t missile_x2;
static uint8_t missile_y2;
static uint8_t missile_dx_p;
static uint8_t missile_dx_n;
static uint8_t missile_dy_p;
static uint8_t missile_dy_n;

static uint8_t asteroid_x1;
static uint8_t asteroid_y1;
static uint8_t asteroid_x2;
static uint8_t asteroid_y2;
static uint8_t asteroid_dx_p;
static uint8_t asteroid_dx_n;
static uint8_t asteroid_dy_p;
static uint8_t asteroid_dy_n;

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
    start = 0;
    end = 0;
    pulses = 0;

    ship_direction = DIRECTION_NONE;
    ship_direction_new = DIRECTION_UP;

    missile_ready = 1;
    missile_x1 = MISSILE_X_INIT;
    missile_x2 = missile_x1;
    missile_y1 = MISSILE_Y_INIT;
    missile_y2 = missile_y1;
    missile_dx_p = 0;
    missile_dx_n = 0;
    missile_dy_p = 0;
    missile_dy_n = 0;

    asteroid_x1 = ASTEROID_X_INIT;
    asteroid_x2 = asteroid_x1;
    asteroid_y1 = ASTEROID_Y_INIT;
    asteroid_y2 = asteroid_y1;
    asteroid_dx_p = 0;
    asteroid_dx_n = 0;
    asteroid_dy_p = ASTEROID_SPEED;
    asteroid_dy_n = 0;

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

void ship_init(void)
{
    sprite_update(0, SHIP_X, SHIP_Y, SPRITE_ON, SHIP_X, SHIP_Y);
}

void ship_up_draw(uint8_t a, int8_t b)
{
    sprite_update(a, SHIP_X, SHIP_Y - 2, b, SHIP_X, SHIP_Y - 2);

    sprite_update(a, SHIP_X + 2, SHIP_Y + 2, b, SHIP_X + 2, SHIP_Y + 2);
    sprite_update(a, SHIP_X - 2, SHIP_Y + 2, b, SHIP_X - 2, SHIP_Y + 2);
}

void ship_down_draw(uint8_t a, int8_t b)
{
    sprite_update(a, SHIP_X, SHIP_Y + 2, b, SHIP_X, SHIP_Y + 2);

    sprite_update(a, SHIP_X + 2, SHIP_Y - 2, b, SHIP_X + 2, SHIP_Y - 2);
    sprite_update(a, SHIP_X - 2, SHIP_Y - 2, b, SHIP_X - 2, SHIP_Y - 2);
}

void ship_left_draw(uint8_t a, int8_t b)
{
    sprite_update(a, SHIP_X - 2, SHIP_Y, b, SHIP_X - 2, SHIP_Y);

    sprite_update(a, SHIP_X + 2, SHIP_Y - 2, b, SHIP_X + 2, SHIP_Y - 2);
    sprite_update(a, SHIP_X + 2, SHIP_Y + 2, b, SHIP_X + 2, SHIP_Y + 2);
}

void ship_right_draw(uint8_t a, int8_t b)
{
    sprite_update(a, SHIP_X + 2, SHIP_Y, b, SHIP_X + 2, SHIP_Y);

    sprite_update(a, SHIP_X - 2, SHIP_Y - 2, b, SHIP_X - 2, SHIP_Y - 2);
    sprite_update(a, SHIP_X - 2, SHIP_Y + 2, b, SHIP_X - 2, SHIP_Y + 2);
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
            ship_up_draw(SPRITE_ON, SPRITE_OFF);
            break;
        case DIRECTION_DOWN:
            ship_down_draw(SPRITE_ON, SPRITE_OFF);
            break;
        case DIRECTION_LEFT:
            ship_left_draw(SPRITE_ON, SPRITE_OFF);
            break;
        case DIRECTION_RIGHT:
            ship_right_draw(SPRITE_ON, SPRITE_OFF);
            break;
    }

    switch (ship_direction_new)
    {
        case DIRECTION_UP:
            ship_up_draw(SPRITE_OFF, SPRITE_ON);
            break;
        case DIRECTION_DOWN:
            ship_down_draw(SPRITE_OFF, SPRITE_ON);
            break;
        case DIRECTION_LEFT:
            ship_left_draw(SPRITE_OFF, SPRITE_ON);
            break;
        case DIRECTION_RIGHT:
            ship_right_draw(SPRITE_OFF, SPRITE_ON);
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

    ship_init();
    ship_update();
    score_draw();
    high_score_draw();

    sprite_update(0, asteroid_x1, asteroid_y1, ASTEROID_SPRITE, asteroid_x2, asteroid_y2);

    while(1)
    {
        sprite_update(MISSILE_SPRITE, missile_x1, missile_y1, MISSILE_SPRITE, missile_x2, missile_y2);

        sprite_update(ASTEROID_SPRITE, asteroid_x1, asteroid_y1, ASTEROID_SPRITE, asteroid_x2, asteroid_y2);

        asteroid_x1 = asteroid_x2;
        asteroid_y1 = asteroid_y2;

        missile_x1 = missile_x2;
        missile_y1 = missile_y2;

        if ((missile_x2 < X_MIN) || (missile_x2 > X_MAX) || (missile_y2 < Y_MIN) || (missile_y2 > Y_MAX))
        {
            missile_ready = 1;
            missile_dx_p = 0;
            missile_dx_n = 0;
            missile_dy_p = 0;
            missile_dy_n = 0;
            sprite_update(MISSILE_SPRITE, missile_x1, missile_y1, 0, missile_x2, missile_y2);

            missile_x1 = MISSILE_X_INIT;
            missile_y1 = MISSILE_Y_INIT;
            missile_x2 = missile_x1;
            missile_y2 = missile_y1;
        }

        missile_x2 += missile_dx_p - missile_dx_n;
        missile_y2 += missile_dy_p - missile_dy_n;

        asteroid_x2 += asteroid_dx_p - asteroid_dx_n;
        asteroid_y2 += asteroid_dy_p - asteroid_dy_n;

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

        if (joystick_fire_get() && missile_ready)
        {
            switch (ship_direction)
            {
                case DIRECTION_UP:
                    missile_dy_n = MISSILE_SPEED;
                    missile_y1 -= MISSILE_OFFSET;
                    break;
                case DIRECTION_DOWN:
                    missile_dy_p = MISSILE_SPEED;
                    missile_y1 += MISSILE_OFFSET;
                    break;
                case DIRECTION_LEFT:
                    missile_dx_n = MISSILE_SPEED;
                    missile_x1 -= MISSILE_OFFSET;
                    break;
                case DIRECTION_RIGHT:
                    missile_dx_p = MISSILE_SPEED;
                    missile_x1 += MISSILE_OFFSET;
                    break;
            }

            missile_ready = 0;
            missile_x2 = missile_x1;
            missile_y2 = missile_y1;
            sprite_update(0, missile_x1, missile_y1, MISSILE_SPRITE, missile_x2, missile_y2);

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
                ship_init();
                ship_up_draw(0, 5);
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
