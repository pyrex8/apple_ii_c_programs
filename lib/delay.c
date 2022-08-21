#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "delay.h"

void delay_100us(void)
{
    __asm__ ("pha");            // 3 cycles
    __asm__ ("tya");            // 2 cycles
    __asm__ ("pha");            // 3 cycles
    __asm__ ("ldy #8");         // 2 cycles

    __asm__ ("loop: nop");      // 2 cycles
    __asm__ ("nop");            // 2 cycles
    __asm__ ("nop");            // 2 cycles
    __asm__ ("dey");            // 2 cycles
    __asm__ ("bne loop");       // 2 cyles

    __asm__ ("pla");            // 3 cycles
    __asm__ ("tay");            // 2 cycles
    __asm__ ("pla");            // 3 cycles
    __asm__ ("nop");            // 2 cycles
}
