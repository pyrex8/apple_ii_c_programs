#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"

#define KEYBOARD_DATA 0xC000
#define KEYBOARD_STROBE 0xC010

uint8_t keyboard_get(void)
{
    uint8_t keypress = PEEK(KEYBOARD_DATA);
    if (keypress > 127)
    {
        STROBE(KEYBOARD_STROBE);
    }
    return keypress;
}
