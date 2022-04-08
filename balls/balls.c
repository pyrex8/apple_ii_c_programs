
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define VHEIGHT 192	// number of scanlines

#define NBALLS          30            // Number of balls to bounce
#define COLUMNS         40            // Number of columns/bytes per row
#define COLUMN_LAST     COLUMNS - 1
#define WHITE           0x7F           // White hires byte
#define BLACK           0x00
#define RKEY            0xD2           // Code for when "R" is pressed
#define RWALL           0xF700
#define LWALL           0x0400
#define BOTTOM          0xB700
#define BOTTOM2         0x6E00         // Bottom * 2, for bouncing

// Addresses
#define BALL            0x06            // Current ball number

#define DATA1           0x26
#define DATA1_P         *((uint8_t*)DATA1)
#define DATA2           0x27
#define DATA2_P         *((uint8_t*)DATA2)
#define ADDR1_L        0x28
#define ADDR1_L_P      *((uint8_t*)ADDR1_L)
#define ADDR1_H        ADDR1_L + 1
#define ADDR1_H_P      *((uint8_t*)ADDR1_H)
#define ADDR2_L        0x2A
#define ADDR2_L_P      *((uint8_t*)ADDR2_L)
#define ADDR2_H        ADDR2_L + 1
#define ADDR2_H_P      *((uint8_t*)ADDR2_H)
#define ADDR3_L        0x2C
#define ADDR3_L_P      *((uint8_t*)ADDR3_L)
#define ADDR3_H        ADDR3_L + 1
#define ADDR3_H_P      *((uint8_t*)ADDR3_H)

#define HCOLOR1         0x1C           // Color value
#define HGRX            0xE0           // two-byte value
#define HGRY            0xE2

#define HGR1SCRN        0x2000         // Start of hires page 1
#define HGR1SCRN_PAGE   0x20
#define HGRSCRN_LENGTH  0x20        // number of pages

#define KBD             0xC000         // key code when MSB set
#define KBDSTRB         0xC010         // clear keyboard buffer
#define TXTCLR          0xC050         // graphics mode
#define TXTSET          0xC051         // text mode
#define LOWSCR          0xC054         // page 1
#define HIRES           0xC057         // hires mode


static const uint8_t LKLO[VHEIGHT] =
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
};

static const uint8_t LKHI[VHEIGHT] =
{
    0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
    0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
    0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
    0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
    0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
    0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
    0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
    0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
    0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
    0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
    0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
    0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
};



static void pageset(uint8_t page, uint8_t value, uint8_t length)
{
    // this method takes 114ms
    ADDR1_L_P = 0x00;
    ADDR1_H_P = page;
    DATA1_P = value;
    DATA2_P = length;

    // init registers with memory
    __asm__ ("lda %b", DATA1);    // value to fill page(s) with
    __asm__ ("ldx %b", DATA2);    // number of pages
    __asm__ ("ldy %b", ADDR1_L); // address two bytes

    // nested loops
    __asm__ ("hclr1: sta (%b),y", ADDR1_L);
    __asm__ ("iny");
    __asm__ ("bne hclr1");
    __asm__ ("inc %b", ADDR1_H);
    __asm__ ("dex");
    __asm__ ("bne hclr1");
}

// ; Draw a horizontal line
// ; A = color byte to repeat, e.g., $7F
// ; Y = row (0-191) ($FF on exit)
// ;
// ; Uses GBASL, GBASH

//static void hline(uint8_t line, uint8_t color)
static void hline(void)
{
    // 580us
    ADDR2_L_P = (uint8_t)LKLO;
    ADDR2_H_P = (uint8_t)(((uint16_t)LKLO)>> 8);
    ADDR3_L_P = (uint8_t)LKHI;
    ADDR3_H_P = (uint8_t)(((uint16_t)LKHI) >> 8);

    // init
    __asm__ ("lda #%b", WHITE);
    __asm__ ("ldy #%b", 0);
    __asm__ ("pha");
    __asm__ ("lda %b,y", ADDR2_L);//     lda LKLO,y
    __asm__ ("sta %b", ADDR1_L);//     sta GBASL
    __asm__ ("lda %b,y", ADDR3_L);//     lda LKHI,y
    __asm__ ("sta %b", ADDR1_H);//     sta GBASH
    __asm__ ("ldy #%b", COLUMN_LAST);//     ldy #COLUMN_LAST  ; Width of screen in bytes
    __asm__ ("pla");//     pla

    // loop
    __asm__ ("hl1: sta (%b),y", ADDR1_L);// hl1:    sta (GBASL),y
    __asm__ ("dey"); //     dey
    __asm__ ("bpl hl1");//     bpl hl1
//     rts
}


static void hclear(void)
{
    pageset(HGR1SCRN_PAGE, BLACK, HGRSCRN_LENGTH);
    STROBE(HIRES);
    STROBE(TXTCLR);
}

void main(void)
{

    hclear();

    TEST_PIN_TOGGLE;
    hline();
    TEST_PIN_TOGGLE;



    while(1)
    {

    }
}
