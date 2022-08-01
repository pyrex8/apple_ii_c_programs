

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "test_pin.h"
#include "zero_page.h"
#include "hires.h"
#include "sprites.h"

static const uint8_t sprites[] =
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x03, 0x00, 0x06, 0x00, 0x0C, 0x00, 0x18, 0x00, 0x30, 0x00, 0x60, 0x00, 0x40, 0x01, 0x00, 0x00,
    0x05, 0x00, 0x0A, 0x00, 0x14, 0x00, 0x28, 0x00, 0x50, 0x00, 0x20, 0x01, 0x40, 0x02, 0x00, 0x00,
    0x0A, 0x00, 0x14, 0x00, 0x28, 0x00, 0x50, 0x00, 0x20, 0x01, 0x40, 0x02, 0x00, 0x05, 0x00, 0x00,
    0x05, 0x00, 0x0A, 0x00, 0x14, 0x00, 0x28, 0x00, 0x50, 0x00, 0x20, 0x01, 0x40, 0x02, 0x00, 0x00,
    0x0A, 0x00, 0x14, 0x00, 0x28, 0x00, 0x50, 0x00, 0x20, 0x01, 0x40, 0x02, 0x00, 0x05, 0x00, 0x00,
    0x0F, 0x00, 0x1E, 0x00, 0x3C, 0x00, 0x78, 0x00, 0x70, 0x01, 0x60, 0x03, 0x40, 0x07, 0x00, 0x00,
    0x15, 0x00, 0x2A, 0x00, 0x54, 0x00, 0x28, 0x01, 0x50, 0x02, 0x20, 0x05, 0x40, 0x0A, 0x00, 0x00,
    0x2A, 0x00, 0x54, 0x00, 0x28, 0x01, 0x50, 0x02, 0x20, 0x05, 0x40, 0x0A, 0x00, 0x15, 0x00, 0x00,
    0x15, 0x00, 0x2A, 0x00, 0x54, 0x00, 0x28, 0x01, 0x50, 0x02, 0x20, 0x05, 0x40, 0x0A, 0x00, 0x00,
    0x2A, 0x00, 0x54, 0x00, 0x28, 0x01, 0x50, 0x02, 0x20, 0x05, 0x40, 0x0A, 0x00, 0x15, 0x00, 0x00,
    0x3F, 0x00, 0x7E, 0x00, 0x7C, 0x01, 0x78, 0x03, 0x70, 0x07, 0x60, 0x0F, 0x40, 0x1F, 0x00, 0x00,
};

static const uint8_t sprites_size[] =   {6, 1, 1, 1, 1, 2, 3, 3, 3, 3, 4, 5, 5, 5, 5, 6};
static const uint8_t sprites_offset[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 ,5, 5, 5, 5};
static const uint8_t sprites_color[] = {0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00};

static uint8_t sprite_buffer[SPRITE_BUFFER_SIZE];
static uint8_t sprite_index;

void sprites_init(void)
{
    SBUFRL_P = (uint8_t)sprite_buffer;
    SBUFRH_P = (uint8_t)(((uint16_t)sprite_buffer)>> 8);
}

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

void sprite_xor(uint8_t sprite, uint8_t column, uint8_t row, uint8_t shift)
{

    // 1150us
    #define SPRITE_CNTR DATA1
    #define SBUFR_IND DATA2
    #define SPRITE_LEFT DATA3
    #define SPRITE_RIGHT DATA4
    #define SPRITE_COLOR DATA5

    sprite_index = (sprite << 4) + (shift << 1);

    DATA1_P = sprites_size[sprite];
    DATA2_P = (row << 2) + column + sprites_offset[sprite];
    DATA3_P = sprites[sprite_index];
    DATA4_P = sprites[sprite_index + 1];
    DATA5_P = sprites_color[sprite];

    // init
    __asm__ ("ldy %b", SBUFR_IND);

    // loop
    __asm__ ("sprite: lda (%b),y", SBUFR);
    __asm__ ("eor %b", SPRITE_LEFT);
    __asm__ ("ora %b", SPRITE_COLOR);
    __asm__ ("sta (%b),y", SBUFR);

    __asm__ ("iny");

    __asm__ ("lda (%b),y", SBUFR);
    __asm__ ("eor %b", SPRITE_RIGHT);
    __asm__ ("ora %b", SPRITE_COLOR);
    __asm__ ("sta (%b),y", SBUFR);

    __asm__ ("iny");
    __asm__ ("iny");
    __asm__ ("iny");

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
