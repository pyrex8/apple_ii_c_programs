
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define VHEIGHT 192	// number of scanlines

#define NBALLS      30            // Number of balls to bounce
#define COLUMNS     40            // Number of columns/bytes per row
#define WHITE       0x7F           // White hires byte
#define RKEY        0xD2           // Code for when "R" is pressed
#define RWALL       0xF700
#define LWALL       0x0400
#define BOTTOM      0xB700
#define BOTTOM2     0x6E00         // Bottom * 2, for bouncing

// Addresses
#define BALL        0x06            // Current ball number

#define DATA1         0x26
#define DATA1_P       *((uint8_t*)DATA1)
#define DATA2         0x27
#define DATA2_P       *((uint8_t*)DATA1)
#define ADDR16_L      0x28
#define ADDR16_L_P    *((uint8_t*)ADDR16_L)
#define ADDR16_H      ADDR16_L + 1
#define ADDR16_H_P    *((uint8_t*)ADDR16_H)

#define HCOLOR1     0x1C           // Color value
#define HGRX        0xE0           // two-byte value
#define HGRY        0xE2

#define HGR1SCRN    0x2000         // Start of hires page 1
#define HGR1SCRN_H  0x20


#define KBD         0xC000         // key code when MSB set
#define KBDSTRB     0xC010         // clear keyboard buffer
#define TXTCLR      0xC050         // graphics mode
#define TXTSET      0xC051         // text mode
#define LOWSCR      0xC054         // page 1
#define HIRES       0xC057         // hires mode


// static const uint8_t LKHI[VHEIGHT] =
// {
//     0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
//     0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
//     0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
//     0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
//     0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
//     0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
//     0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
//     0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
//     0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
//     0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
//     0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
//     0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
// };
//
// static const uint8_t LKLO[VHEIGHT] =
// {
//     0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
//     0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
//     0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
//     0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
//     0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
//     0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
//     0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
//     0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
//     0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
//     0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
//     0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
//     0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
// };


static void pageset(uint8_t page, uint8_t value, uint8_t length)
{
    // this method takes 114ms
    ADDR16_L_P = 0x00;
    ADDR16_H_P = page;
    DATA1_P = value;
    DATA2_P = length;

    STROBE(HIRES);
    STROBE(TXTCLR);
    TEST_PIN_TOGGLE;

    // init registers with memory
    __asm__ ("ldx %b", DATA2);    // number of pages
    __asm__ ("ldy %b", ADDR16_L); // address two bytes
    __asm__ ("lda %b", DATA1);    // value to fill page(s) with

    // nested loops
    __asm__ ("hclr1: sta (%b),y", ADDR16_L);
    __asm__ ("iny");
    __asm__ ("bne hclr1");
    __asm__ ("inc %b", ADDR16_H);
    __asm__ ("dex");
    __asm__ ("bne hclr1");

    TEST_PIN_TOGGLE;
}


void main(void)
{
    pageset(0x20, 0xFF, 0x20);

    while(1)
    {

    }
}
