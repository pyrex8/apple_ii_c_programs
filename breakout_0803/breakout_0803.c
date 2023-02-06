
// Note all timing is based on the slower emulated apple IIe that is taking
// approx 1.5us per cycle.

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../lib/test_pin.h"
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
#define SOUND_PULSES 10
#define SOUND_DELAY 6

#define PADDLE_SPRITE 10
#define SPRITE_STEP 6
#define SPRITE_STEP2 (2 * SPRITE_STEP)
#define PADDLE_X_MIN 58
#define PADDLE_X_MAX 188
#define PADDLE_Y 180
#define PADDLE_X_INIT PADDLE_X_MIN + (SPRITE_STEP * 10)
#define PADDLE_WIDTH 18
#define PADDLE_CENTER 4
#define PADDLE_COLLISION_MIN (PADDLE_Y - PADDLE_CENTER)
#define PADDLE_COLLISION_MAX PADDLE_Y
#define PADDLE_UPDATE_DELAY 3


#define BALL_SPRITE 10
#define BALL_SPEED_X 2
#define BALL_SPEED_Y 8
#define BALL_X_INIT 112
#define BALL_Y_INIT 112
#define BALL_X_MIN 48
#define BALL_X_MAX 200
#define BALL_Y_MIN 5
#define BALL_Y_MAX 182

#define BRICKS_NUMBER 20
#define BRICKS_SPACING 3
#define BRICKS_BLUE 14
#define BRICKS_ORANGE 13
#define BRICKS_PURPLE 12
#define BRICKS_GREEN 11
#define BRICKS_WHITE 10
#define BRICK_X_OFFSET 47
#define BRICK_Y_OFFSET 5
#define BRICKS_Y_BLUE BRICKS_SPACING - 1
#define BRICKS_Y_ORANGE (BRICKS_Y_BLUE + BRICKS_SPACING)
#define BRICKS_Y_PURPLE (BRICKS_Y_ORANGE + BRICKS_SPACING)
#define BRICKS_Y_GREEN (BRICKS_Y_PURPLE + BRICKS_SPACING)
#define BRICKS_Y_WHITE (BRICKS_Y_GREEN + BRICKS_SPACING)

#define SCORE_Y 10

static uint8_t pulses;
static uint8_t paddle_x1;
static uint8_t paddle_x2;
static uint8_t paddle_update;
static uint8_t ball_x1;
static uint8_t ball_x2;
static uint8_t ball_y1;
static uint8_t ball_y2;
static uint8_t ball_dx_p;
static uint8_t ball_dx_n;
static uint8_t ball_dx_boost_p;
static uint8_t ball_dx_boost_n;
static uint8_t ball_dy_p;
static uint8_t ball_dy_n;
static uint8_t ball_speed_x;
static uint8_t ball_speed_y;
static uint8_t x_contract;
static uint8_t y_contract;
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

// pad the end with one zero
static uint8_t bricks_blue[BRICKS_NUMBER + 1] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
static uint8_t bricks_purple[BRICKS_NUMBER + 1] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
static uint8_t bricks_orange[BRICKS_NUMBER + 1] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
static uint8_t bricks_green[BRICKS_NUMBER + 1] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};

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

uint8_t expand_x(uint8_t x)
{
    return (x << 3) + BRICK_X_OFFSET;
}

uint8_t expand_y(uint8_t y)
{
    return (y << 3) + BRICK_Y_OFFSET;
}

uint8_t contract_x(uint8_t x)
{
    return (x - BRICK_X_OFFSET + 4) >> 3;
}

uint8_t contract_y(uint8_t y)
{
    return (y - BRICK_Y_OFFSET + 4) >> 3;
}

void brick_on(uint8_t x, uint8_t y, uint8_t sprite)
{
    uint8_t x1 = expand_x(x);
    uint8_t y1 = expand_y(y);
    sprite_update(0, x1, y1, sprite, x1, y1);
}

void brick_off(uint8_t x, uint8_t y, uint8_t sprite)
{
    uint8_t x1 = expand_x(x);
    uint8_t y1 = expand_y(y);
    sprite_update(sprite, x1, y1, 0, x1, y1);
}

void blocks(void)
{
    uint8_t i;
    for (i = 0; i < BRICKS_NUMBER; i++)
    {
        bricks_blue[i] = 1;
        brick_on(i, BRICKS_Y_BLUE, BRICKS_BLUE);

        bricks_orange[i] = 1;
        brick_on(i, BRICKS_Y_ORANGE, BRICKS_ORANGE);

        bricks_purple[i] = 1;
        brick_on(i, BRICKS_Y_PURPLE, BRICKS_PURPLE);

        bricks_green[i] = 1;
        brick_on(i, BRICKS_Y_GREEN, BRICKS_GREEN);
    }
}

void game_init(void)
{
    paddle_x1 = PADDLE_X_INIT;
    paddle_x2 = paddle_x1;

    ball_x1 = BALL_X_INIT;
    ball_x2 = ball_x1;
    ball_y1 = BALL_Y_INIT;
    ball_y2 = ball_y1;
    ball_speed_x = BALL_SPEED_X;
    ball_speed_y = BALL_SPEED_Y;
    ball_dx_p = ball_speed_x;
    ball_dx_n = 0;
    ball_dx_boost_p = 0;
    ball_dx_boost_n = 0;
    ball_dy_p = 0;
    ball_dy_n = 0;

    x_contract = 0;
    y_contract = 0;

    start = 0;
    end = 0;
    pulses = 0;

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
    digit_set(10, SCORE_Y, score_hundreds);
    digit_set(11, SCORE_Y, score_tens);
    digit_set(12, SCORE_Y, score_ones);
}

void high_score_draw(void)
{
    digit_set(27, SCORE_Y, high_hundreds);
    digit_set(28, SCORE_Y, high_tens);
    digit_set(29, SCORE_Y, high_ones);
}

void paddle_draw(void)
{
    sprite_update(0, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x2, PADDLE_Y);
    sprite_update(0, paddle_x1 - SPRITE_STEP, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - SPRITE_STEP, PADDLE_Y);
    sprite_update(0, paddle_x1 + SPRITE_STEP, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + SPRITE_STEP, PADDLE_Y);
    sprite_update(0, paddle_x1 - SPRITE_STEP2, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - SPRITE_STEP2, PADDLE_Y);
    sprite_update(0, paddle_x1 + SPRITE_STEP2, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + SPRITE_STEP2, PADDLE_Y);
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

    sprite_update(0, ball_x1, ball_y1, BALL_SPRITE, ball_x2, ball_y2);

    blocks();
    paddle_draw();
    score_draw();
    high_score_draw();

    while(1)
    {
        if (paddle_x2 > paddle_x1)
         {
             sprite_update(PADDLE_SPRITE, paddle_x1 - SPRITE_STEP2, PADDLE_Y, 0, paddle_x1 - SPRITE_STEP2, PADDLE_Y);
             sprite_update(0, paddle_x2 + SPRITE_STEP2, PADDLE_Y, PADDLE_SPRITE, paddle_x2 + SPRITE_STEP2, PADDLE_Y);
         }
         if (paddle_x2 < paddle_x1)
         {
             sprite_update(PADDLE_SPRITE, paddle_x1 + SPRITE_STEP2, PADDLE_Y, 0, paddle_x1 + SPRITE_STEP2, PADDLE_Y);
             sprite_update(0, paddle_x2 - SPRITE_STEP2, PADDLE_Y, PADDLE_SPRITE, paddle_x2 - SPRITE_STEP2, PADDLE_Y);
         }
         if (paddle_x2 == paddle_x1)
         {
             sprite_update(PADDLE_SPRITE, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x1, PADDLE_Y);
             sprite_update(PADDLE_SPRITE, paddle_x1, PADDLE_Y, PADDLE_SPRITE, paddle_x1, PADDLE_Y);
         }


         paddle_x1 = paddle_x2;

        if (end == 0)
        {
            sprite_update(BALL_SPRITE, ball_x1, ball_y1, BALL_SPRITE, ball_x2, ball_y2);
        }

        ball_x1 = ball_x2;
        ball_y1 = ball_y2;

        if (ball_x2 > BALL_X_MAX)
        {
            ball_dx_p = 0;
            ball_dx_n = ball_speed_x;
            ball_dx_boost_p = 0;
            ball_dx_boost_n = 0;
            pulses = SOUND_BOUNCE;
        }

        if (ball_x2 < BALL_X_MIN)
        {
            ball_dx_p = ball_speed_x;
            ball_dx_n = 0;
            ball_dx_boost_p = 0;
            ball_dx_boost_n = 0;
            pulses = SOUND_BOUNCE;
        }

        if (ball_y2 > BALL_Y_MAX)
        {
            ball_dx_p = 0;
            ball_dx_n = 0;
            ball_dy_p = 0;
            ball_dy_n = 0;
            ball_dx_boost_p = 0;
            ball_dx_boost_n = 0;
            pulses = SOUND_END;
            end = 1;
            sprite_update(BALL_SPRITE, ball_x1, ball_y1, 0, ball_x2, ball_y2);
            ball_y2 = BALL_Y_MAX;
            if (ball_x2 > BALL_X_MAX)
            {
                ball_x2 = BALL_X_MAX;
            }
            if (ball_x2 < BALL_X_MIN)
            {
                ball_x2 = BALL_X_MIN;
            }
        }

        if ((ball_y2 >= PADDLE_COLLISION_MIN) && (ball_y2 < PADDLE_COLLISION_MAX))
        {
            if ((ball_x2 > paddle_x2 - PADDLE_WIDTH) && (ball_x2 < paddle_x2 + PADDLE_WIDTH))
            {
                ball_dy_p = 0;
                ball_dy_n = ball_speed_y;
                pulses = SOUND_BOUNCE;
                if (ball_x2 < paddle_x2 - PADDLE_CENTER)
                {
                    ball_dx_boost_p = 0;
                    ball_dx_boost_n = 1;
                }
                else
                {
                    if (ball_x2 > paddle_x2 + PADDLE_CENTER)
                    {
                        ball_dx_boost_p = 1;
                        ball_dx_boost_n = 0;
                    }
                    else
                    {
                        ball_dx_boost_p = 0;
                        ball_dx_boost_n = 0;
                    }
                }

            }
        }

        if (ball_y2 < BALL_Y_MIN)
        {
            ball_dy_p = ball_speed_y;
            ball_dy_n = 0;
            pulses = SOUND_BOUNCE;
        }

        ball_x2 += ball_dx_p - ball_dx_n + ball_dx_boost_p - ball_dx_boost_n;
        ball_y2 += ball_dy_p - ball_dy_n;

        x_contract = contract_x(ball_x2);
        y_contract = contract_y(ball_y2);

        if (y_contract == BRICKS_Y_BLUE)
        {
            if (bricks_blue[x_contract] == 1)
            {
                bricks_blue[x_contract] = 0;
                ball_dy_p = ball_speed_y - ball_dy_p;
                ball_dy_n = ball_speed_y - ball_dy_n;
                pulses = SOUND_BOUNCE;
                brick_off(x_contract, y_contract, BRICKS_BLUE);
                score_increase();
            }
        }
        if (y_contract == BRICKS_Y_ORANGE)
        {
            if (bricks_orange[x_contract] == 1)
            {
                bricks_orange[x_contract] = 0;
                ball_dy_p = ball_speed_y - ball_dy_p;
                ball_dy_n = ball_speed_y - ball_dy_n;
                pulses = SOUND_BOUNCE;
                brick_off(x_contract, y_contract, BRICKS_ORANGE);
                score_increase();
            }
        }
        if (y_contract == BRICKS_Y_PURPLE)
        {
            if (bricks_purple[x_contract] == 1)
            {
                bricks_purple[x_contract] = 0;
                ball_dy_p = ball_speed_y - ball_dy_p;
                ball_dy_n = ball_speed_y - ball_dy_n;
                pulses = SOUND_BOUNCE;
                brick_off(x_contract, y_contract, BRICKS_PURPLE);
                score_increase();
            }
        }
        if (y_contract == BRICKS_Y_GREEN)
        {
            if (bricks_green[x_contract] == 1)
            {
                bricks_green[x_contract] = 0;
                ball_dy_p = ball_speed_y - ball_dy_p;
                ball_dy_n = ball_speed_y - ball_dy_n;
                pulses = SOUND_BOUNCE;
                brick_off(x_contract, y_contract, BRICKS_GREEN);
                score_increase();
            }
        }

        joystick_run();

        paddle_update++;
        if (paddle_update == PADDLE_UPDATE_DELAY)
        {
            paddle_update = 0;
            if (joystick_left_get())
            {
                if (paddle_x2 > PADDLE_X_MIN)
                {
                    paddle_x2 -= SPRITE_STEP;
                }
            }

            if (joystick_right_get())
            {
                if (paddle_x2 < PADDLE_X_MAX)
                {
                    paddle_x2 += SPRITE_STEP;
                }
            }
        }

        if (joystick_fire_get())
        {
            if (start == 0)
            {
                start = 1;
                ball_speed_y = 2;
                ball_dy_p = ball_speed_y;
            }
            if (end == 1)
            {
                game_init();
                hires_clr();
                hbox();
                sprite_update(0, ball_x1, ball_y1, BALL_SPRITE, ball_x2, ball_y2);
                blocks();
                paddle_draw();
                score_draw();
                high_score_draw();
            }
        }

        if (pulses > 0)
        {
            sound(SOUND_PULSES, SOUND_DELAY);
            pulses--;
        }
    }
}
