
#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"

#define JOYSTICK_BUTTON0_ADDR 0xC061
#define JOYSTICK_BUTTON1_ADDR 0xC062
#define JOYSTICK_PADDLE0_ADDR 0xC064
#define JOYSTICK_PADDLE1_ADDR 0xC065

#define JOYSTICK_TIMER_RESET_ADDR 0xC070

#define JOYSTICK_FIRE (PEEK(JOYSTICK_BUTTON0_ADDR) > 127 ? 1 : 0)
#define JOYSTICK_START POKE(JOYSTICK_TIMER_RESET_ADDR, 0)
#define JOYSTICK_LEFT (PEEK(JOYSTICK_PADDLE0_ADDR) > 127 ? 0 : 1)
#define JOYSTICK_RIGHT (PEEK(JOYSTICK_PADDLE0_ADDR) > 127 ? 1 : 0)
#define JOYSTICK_UP (PEEK(JOYSTICK_PADDLE1_ADDR) > 127 ? 0 : 1)
#define JOYSTICK_DOWN (PEEK(JOYSTICK_PADDLE1_ADDR) > 127 ? 1 : 0)

uint8_t left = 0;
uint8_t right = 0;
uint8_t up = 0;
uint8_t down = 0;

static void delay(void)
{
    uint8_t i = 0;
    for (i = 0; i < 50; i++)
    {
    }
}

void joystick_run(void)
{

    JOYSTICK_START;
    delay();
    left = JOYSTICK_LEFT;
    up = JOYSTICK_UP;
    TEST_PIN_TOGGLE;
    delay();
    TEST_PIN_TOGGLE;
    right = JOYSTICK_RIGHT;
    down = JOYSTICK_DOWN;
}

uint8_t joystick_fire_get(void)
{
    return JOYSTICK_FIRE;
}

uint8_t joystick_up_get(void)
{
    return up;
}

uint8_t joystick_down_get(void)
{
    return down;
}

uint8_t joystick_left_get(void)
{
    return left;
}

uint8_t joystick_right_get(void)
{
    return right;
}
