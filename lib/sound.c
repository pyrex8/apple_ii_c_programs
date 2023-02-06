#include <stdio.h>
#include <stdint.h>
#include "delay.h"
#include "test_pin.h"

#define SPEAKER_ADDR 0xC030
#define SOUND_TOGGLE POKE(SPEAKER_ADDR, 0)

void sound(uint8_t pulses, uint8_t delay)
{
    uint8_t i = 0;
    uint8_t j = 0;
    for (i = 0; i < pulses; i++)
    {
        SOUND_TOGGLE;
        for (j = 0; j < delay; j++)
        {
            delay_100us();
        }
    }
}
