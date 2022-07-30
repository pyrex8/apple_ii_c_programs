

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"
#include "zero_page.h"
#include "hires.h"
#include "sprites.h"

void sprite_hgr_to_buffer(uint8_t column, uint8_t row)
{
    // 1580us
    #define SBUFR_INDEX DATA1
    #define HGR_COL_START DATA2
    #define HGR_COL DATA3
    #define HGR_ROW DATA4

    DATA1_P = 0;
    DATA2_P = column;
    DATA3_P = column;
    DATA4_P = row;

    // new row
    __asm__ ("newrow: lda %b", HGR_COL_START);
    __asm__ ("sta %b", HGR_COL);
    __asm__ ("inc %b", HGR_ROW);
    // Get the row address
    __asm__ ("ldy %b", HGR_ROW);
    __asm__ ("lda (%b),y", LKLO);
    __asm__ ("sta %b", ADDR1L);
    __asm__ ("lda (%b),y", LKHI);
    __asm__ ("sta %b", ADDR1H);

    // get byte from screen memory
    __asm__ ("newcol: ldy %b", HGR_COL);
    __asm__ ("lda (%b),y", ADDR1L);
    // store in sprite_buffer
    __asm__ ("ldy %b", SBUFR_INDEX);
    __asm__ ("sta (%b),y", SBUFR);
    // decrement counters
    __asm__ ("inc %b", HGR_COL);
    __asm__ ("inc %b", SBUFR_INDEX);

    // test for new row
    __asm__ ("lda %b", SBUFR_INDEX);
    __asm__ ("and #%b", 3);
//    __asm__ ("cmp #%b", 3);
    __asm__ ("beq newrow");

    __asm__ ("lda %b", SBUFR_INDEX);
    __asm__ ("cmp #%b", 41);
    __asm__ ("bne newcol");
}

void sprite_xor(uint8_t page, uint8_t sprite, uint8_t column, uint8_t row, uint8_t shift)
{

    // 1150us
    #define SPRITE_CNTR DATA1
    #define SBUFR_IND DATA2
    #define SPRITE_SHFT DATA3
    #define SPRITE_NUM DATA4
    #define PAGE DATA5

    DATA1_P = 8;
    DATA2_P = (row << 2) + column;
    DATA3_P = shift << 4;
    DATA4_P = sprite;
    DATA5_P = page;

    // init
    __asm__ ("lda %b", STABLEL);
    __asm__ ("clc");
    __asm__ ("adc %b", SPRITE_NUM);
    __asm__ ("sta %b", SPRITEL);

    __asm__ ("lda %b", STABLEH);
    __asm__ ("adc %b", PAGE);
    __asm__ ("sta %b", SPRITEH);

    // loop
    __asm__ ("sprite: ldy %b", SBUFR_IND);
    __asm__ ("lda (%b),y", SBUFR);
    __asm__ ("ldy %b", SPRITE_SHFT);
    __asm__ ("eor (%b), y", SPRITE);
    __asm__ ("ldy %b", SBUFR_IND);
    __asm__ ("sta (%b),y", SBUFR);

    __asm__ ("inc %b", SPRITE_SHFT);
    __asm__ ("inc %b", SBUFR_IND);

    __asm__ ("ldy %b", SBUFR_IND);
    __asm__ ("lda (%b),y", SBUFR);
    __asm__ ("ldy %b", SPRITE_SHFT);
    __asm__ ("eor (%b), y", SPRITE);
    __asm__ ("ldy %b", SBUFR_IND);
    __asm__ ("sta (%b),y", SBUFR);

    __asm__ ("inc %b", SPRITE_SHFT);

    __asm__ ("inc %b", SBUFR_IND);
    __asm__ ("inc %b", SBUFR_IND);
    __asm__ ("inc %b", SBUFR_IND);

    __asm__ ("dec %b", SPRITE_CNTR);
    __asm__ ("bne sprite");
}

void sprite_buffer_to_hgr(uint8_t column, uint8_t row)
{
    // 1580us
    #define SBUFR_INDEX DATA1
    #define HGR_COL_START DATA2
    #define HGR_COL DATA3
    #define HGR_ROW DATA4

    DATA1_P = 0;
    DATA2_P = column;
    DATA3_P = column;
    DATA4_P = row;

    // new row
    __asm__ ("newrow: lda %b", HGR_COL_START);
    __asm__ ("sta %b", HGR_COL);
    __asm__ ("inc %b", HGR_ROW);
    // Get the row address
    __asm__ ("ldy %b", HGR_ROW);
    __asm__ ("lda (%b),y", LKLO);
    __asm__ ("sta %b", ADDR1L);
    __asm__ ("lda (%b),y", LKHI);
    __asm__ ("sta %b", ADDR1H);

    // get byte from sprite_buffer
    __asm__ ("newcol: ldy %b", SBUFR_INDEX);
    __asm__ ("lda (%b),y", SBUFR);
    // store in screen memory
    __asm__ ("ldy %b", HGR_COL);
    __asm__ ("sta (%b),y", ADDR1L);
    // decrement counters
    __asm__ ("inc %b", HGR_COL);
    __asm__ ("inc %b", SBUFR_INDEX);

    // test for new row
    __asm__ ("lda %b", SBUFR_INDEX);
    __asm__ ("and #%b", 3);
    __asm__ ("beq newrow");

    __asm__ ("lda %b", SBUFR_INDEX);
    __asm__ ("cmp #%b", 41);
    __asm__ ("bne newcol");

}
