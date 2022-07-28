

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"
#include "zero_page.h"
#include "hires.h"


void hires_clr(void)
{
    // 77.9ms
    memset((uint8_t *)HGR1SCRN, 0, HGR_SCRN_LEN); // clear page 1
    STROBE(HIRES);
    STROBE(TXTCLR);
}

void hires_hline(uint8_t column, uint8_t row, uint8_t length, uint8_t pixels)
{
    // 1.7ms
    DATA1_P = pixels;
    DATA2_P = row;
    DATA3_P = column;
    DATA4_P = column + length;

    // init
    __asm__ ("ldy %b", DATA2);
    __asm__ ("lda (%b),y", LKLO);       // Get the row address
    __asm__ ("sta %b", ADDR1L);
    __asm__ ("lda (%b),y", LKHI);
    __asm__ ("sta %b", ADDR1H);

    __asm__ ("ldy %b", DATA4);

    // loop
    __asm__ ("hl1: dey");
    __asm__ ("lda (%b),y", ADDR1L);
    __asm__ ("eor %b", DATA1);
    __asm__ ("sta (%b),y", ADDR1L);
    __asm__ ("cpy %b", DATA3);
    __asm__ ("bne hl1");
}

void hires_vline(uint8_t column, uint8_t row, uint8_t length, uint8_t pixels)
{
    // 11.2ms
    DATA1_P = pixels;
    DATA2_P = column;
    DATA3_P = row;
    DATA4_P = row + length;

    // init
    __asm__ ("ldx %b", DATA4);          // Start at second-to-last row

    // loop
    __asm__ ("vl1: dex");
    __asm__ ("txa");               // row to a
    __asm__ ("tay");                    // row to y
    __asm__ ("lda (%b),y", LKLO);       // Get the row address
    __asm__ ("sta %b", ADDR1L);
    __asm__ ("lda (%b),y", LKHI);
    __asm__ ("sta %b", ADDR1H);
    __asm__ ("ldy %b", DATA2);          // column
    __asm__ ("lda (%b),y", ADDR1L);
    __asm__ ("eor %b", DATA1);
    __asm__ ("sta (%b),y", ADDR1L);
    __asm__ ("cpx %b", DATA3);
    __asm__ ("bne vl1");
}
