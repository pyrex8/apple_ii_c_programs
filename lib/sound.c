#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"

#define SPEAKER_ADDR 0xC030
#define SOUND_TOGGLE POKE(SPEAKER_ADDR, 0)

void sound(uint8_t pulses)
{
    uint8_t i = 0;
    for (i = 0; i < pulses; i++)
    {
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
    }
}
