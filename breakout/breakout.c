
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


static const uint8_t lklo[] = {HIRES_MEMORY_LOW_BYTE};
static const uint8_t lkhi[] = {HIRES_MEMORY_HIGH_BYTE};
const uint8_t div7[] = {DIV7};
const uint8_t mod7[] = {MOD7};

#define SPRITE_XH_CALC(x) (div7[x])
#define SPRITE_XL_CALC(x) (mod7[x])

#define BYTE_HIGH_BITS 8
#define SOUND_PULESES 5
#define COLOR_OFFSET_ORANGE_GREEN 2

#define BALL_SPRITE 10
#define BALL_SPEED 4
#define BALL_X_INIT 112
#define BALL_Y_INIT 112
#define BALL_X_MIN 48
#define BALL_X_MAX 200
#define BALL_Y_MIN 5
#define BALL_Y_MAX 180

#define PADDLE_SPRITE 10
#define PADDLE_SPRITE_SPACING 4
#define PADDLE_STEP 4
#define PADDLE_Y 180
#define PADDLE_X_INIT 140

#define BRICKS_NUMBER 20
#define BRICKS_SPACING 8
#define BRICKS_BLUE 13
#define BRICKS_PURPLE 11
#define BRICKS_ORANGE 14
#define BRICKS_GREEN 12
#define BRICKS_Y_BLUE 20
#define BRICKS_Y_PURPLE 40
#define BRICKS_Y_ORANGE 60
#define BRICKS_Y_GREEN 80

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
static uint8_t bricks_blue[BRICKS_NUMBER] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint8_t bricks_purple[BRICKS_NUMBER] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint8_t bricks_orange[BRICKS_NUMBER] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static uint8_t bricks_green[BRICKS_NUMBER] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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
    hires_hline(8, 6, 24, WHITE);
    hires_hline(8, 6 + 1, 24, WHITE);
    hires_vline(8, 8, 191, 0x03);
    hires_vline(31, 8, 191, 0x60);
}

void blocks(void)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    for (i = 0; i < BRICKS_NUMBER; i++)
    {
        j = BRICKS_SPACING * i + BALL_X_MIN;
        k = j - COLOR_OFFSET_ORANGE_GREEN;
        if (bricks_blue[i])
        {
            sprite_update(0, j, BRICKS_Y_BLUE, BRICKS_BLUE, j, BRICKS_Y_BLUE);
        }

        if (bricks_purple[i])
        {
            sprite_update(0, j, BRICKS_Y_PURPLE, BRICKS_PURPLE, j, BRICKS_Y_PURPLE);
        }

        if (bricks_orange[i])
        {
            sprite_update(0, k, BRICKS_Y_ORANGE, BRICKS_ORANGE, k, BRICKS_Y_ORANGE);
        }

        if (bricks_green[i])
        {
            sprite_update(0, k, BRICKS_Y_GREEN, BRICKS_GREEN, k, BRICKS_Y_GREEN);
        }
    }
}

void ball_x_quant_on(uint8_t x)
{
    uint8_t x1 = (((x - BALL_X_MIN) >> 3) << 3) + BALL_X_MIN;
    sprite_update(0, x1, 10, BALL_SPRITE, x1, 10);
}

void ball_x_quant_off(uint8_t x)
{
    uint8_t x1 = (((x - BALL_X_MIN) >> 3) << 3) + BALL_X_MIN;
    sprite_update(BALL_SPRITE, x1, 10, 0, x1, 10);
}

void ball_y_quant_on(uint8_t y)
{
    uint8_t y1 = (((y - 5) >> 3) << 3) + 5;
    sprite_update(0, 10, y1, BALL_SPRITE, 10, y1);
}

void ball_y_quant_off(uint8_t y)
{
    uint8_t y1 = (((y - 5) >> 3) << 3) + 5;
    sprite_update(BALL_SPRITE, 10, y1, 0, 10, y1);
}

void main(void)
{
    paddle_x1 = PADDLE_X_INIT;
    paddle_x2 = paddle_x1;

    ball_x1 = BALL_X_INIT;
    ball_x2 = ball_x1;
    ball_y1 = BALL_X_INIT;
    ball_y2 = ball_y1;
    ball_speed_x = BALL_SPEED;
    ball_speed_y = 0;
    ball_dx_p = ball_speed_x;
    ball_dx_n = 0;
    ball_dy_p = 0;
    ball_dy_n = 0;

    start = 0;

    pointers_init();
    sprites_init();
    hires_clr();
    hbox();


    sprite_update(0, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x2, PADDLE_Y);
    sprite_update(0, paddle_x1 - PADDLE_SPRITE_SPACING, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - PADDLE_SPRITE_SPACING, PADDLE_Y);
    sprite_update(0, paddle_x1 + PADDLE_SPRITE_SPACING, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + PADDLE_SPRITE_SPACING, PADDLE_Y);

    sprite_update(0, ball_x1, ball_y1, BALL_SPRITE, ball_x2, ball_y2);

    blocks();

    while(1)
    {

        if (paddle_x2 > paddle_x1)
        {
            sprite_update(PADDLE_SPRITE, paddle_x1 - PADDLE_SPRITE_SPACING, PADDLE_Y, 0, paddle_x1 - PADDLE_SPRITE_SPACING, PADDLE_Y);
            sprite_update(0, paddle_x2 + PADDLE_SPRITE_SPACING, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + PADDLE_SPRITE_SPACING, PADDLE_Y);
        }
        if (paddle_x2 < paddle_x1)
        {
            sprite_update(PADDLE_SPRITE, paddle_x1 + PADDLE_SPRITE_SPACING, PADDLE_Y, 0, paddle_x1 + PADDLE_SPRITE_SPACING, PADDLE_Y);
            sprite_update(0, paddle_x2 - PADDLE_SPRITE_SPACING, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - PADDLE_SPRITE_SPACING, PADDLE_Y);
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

        if (ball_x2 > BALL_X_MAX)
        {
            ball_dx_p = 0;
            ball_dx_n = ball_speed_x;
            pulses = SOUND_PULESES;
        }

        if (ball_x2 < BALL_X_MIN)
        {
            ball_dx_p = ball_speed_x;
            ball_dx_n = 0;
            pulses = SOUND_PULESES;
        }

        if (ball_y2 > BALL_Y_MAX)
        {
            ball_dy_p = 0;
            ball_dy_n = ball_speed_y;
            pulses = SOUND_PULESES;
        }

        if (ball_y2 < BALL_Y_MIN)
        {
            ball_dy_p = ball_speed_y;
            ball_dy_n = 0;
            pulses = SOUND_PULESES;
        }

        ball_x_quant_off(ball_x2);
        ball_y_quant_off(ball_y2);

        ball_x2 += ball_dx_p - ball_dx_n;
        ball_y2 += ball_dy_p - ball_dy_n;

        ball_x_quant_on(ball_x2);
        ball_y_quant_on(ball_y2);

        joystick_run();

        if (joystick_left_get())
        {
            if (paddle_x2 > 50)
            {
                paddle_x2 -= PADDLE_STEP;
            }
        }

        if (joystick_right_get())
        {
            if (paddle_x2 < 200)
            {
                paddle_x2 += PADDLE_STEP;
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
