

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"
#include "hires.h"


void hires_clr(void)
{
    // 77.9ms
    memset((uint8_t *)HGR1SCRN, 0, HGR_SCRN_LEN); // clear page 1
    STROBE(HIRES);
    STROBE(TXTCLR);
}
