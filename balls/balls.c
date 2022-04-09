
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define ROWS            192	// number of scanlines
#define ROW_FIRST       0
#define ROW_LAST        ROWS - 1
#define ROW_SECOND_LAST ROW_LAST - 1
#define NBALLS          30            // Number of balls to bounce
#define COLUMNS         40            // Number of columns/bytes per row
#define COLUMN_FIRST    0
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
#define DATA3           0x28
#define DATA3_P         *((uint8_t*)DATA3)
#define ADDR1_L         0x29
#define ADDR1_L_P       *((uint8_t*)ADDR1_L)
#define ADDR1_H         ADDR1_L + 1
#define ADDR1_H_P       *((uint8_t*)ADDR1_H)
#define ADDR2_L         0x2B
#define ADDR2_L_P       *((uint8_t*)ADDR2_L)
#define ADDR2_H         ADDR2_L + 1
#define ADDR2_H_P       *((uint8_t*)ADDR2_H)
#define ADDR3_L         0x2D
#define ADDR3_L_P       *((uint8_t*)ADDR3_L)
#define ADDR3_H         ADDR3_L + 1
#define ADDR3_H_P       *((uint8_t*)ADDR3_H)

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


static const uint8_t LKLO[ROWS] =
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

static const uint8_t LKHI[ROWS] =
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




static const uint8_t BALL0[] =
{
    0X3C, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X3C,
    0X78, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X78,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};
//     00111100, 01111111, 01111111, 01111111, 01111111, 01111111, 01111111, 00111100,
//     01111000, 01111110, 01111110, 01111110, 01111110, 01111110, 01111110, 01111000,
//     01110000, 01111100, 01111100, 01111100, 01111100, 01111100, 01111100, 01110000,
//     01100000, 01111000, 01111000, 01111000, 01111000, 01111000, 01111000, 01100000,
//     01000000, 01110000, 01110000, 01110000, 01110000, 01110000, 01110000, 01000000,
//     00000000, 01100000, 01100000, 01100000, 01100000, 01100000, 01100000, 00000000,
//     00000000, 01000000, 01000000, 01000000, 01000000, 01000000, 01000000, 00000000,
// };

static const uint8_t BALL1[] =
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};
// static const uint8_t BALL1[8]
// {
//     0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
// };
//     00000000, 00000001, 00000001, 00000001, 00000001, 00000001, 00000001, 00000000,
//     00000000, 00000011, 00000011, 00000011, 00000011, 00000011, 00000011, 00000000,
//     00000001, 00000111, 00000111, 00000111, 00000111, 00000111, 00000111, 00000001,
//     00000011, 00001111, 00001111, 00001111, 00001111, 00001111, 00001111, 00000011,
//     00000111, 00011111, 00011111, 00011111, 00011111, 00011111, 00011111, 00000111,
//     00001111, 00111111, 00111111, 00111111, 00111111, 00111111, 00111111, 00001111,
//     00011110, 01111111, 01111111, 01111111, 01111111, 01111111, 01111111, 00011110,
// };


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

static void hline(uint8_t line, uint8_t pixels)
{
    // Assembly = 580us, C = 850us
    DATA1_P = pixels;
    ADDR1_L_P = LKLO[line];
    ADDR1_H_P = LKHI[line];

    // init
    __asm__ ("ldy #%b", COLUMN_LAST); // Width of screen in bytes
    __asm__ ("lda %b", DATA1);

    // loop
    __asm__ ("hl1: sta (%b),y", ADDR1_L);
    __asm__ ("dey");
    __asm__ ("bpl hl1");
}

static void vline(uint8_t column, uint8_t pixels)
{
    // Assembly = 6700us, C = 9400us
    DATA1_P = pixels;
    DATA2_P = column;
    DATA3_P = ROW_SECOND_LAST;
    ADDR2_L_P = (uint8_t)LKLO;
    ADDR2_H_P = (uint8_t)(((uint16_t)LKLO)>> 8);
    ADDR3_L_P = (uint8_t)LKHI;
    ADDR3_H_P = (uint8_t)(((uint16_t)LKHI) >> 8);

    // init
    __asm__ ("ldx %b", DATA3);          // Start at second-to-last row

    // loop
    __asm__ ("vl1: txa");               // row to a
    __asm__ ("tay");                    // row to y
    __asm__ ("lda (%b),y", ADDR2_L); // Get the row address
    __asm__ ("sta %b", ADDR1_L);
    __asm__ ("lda (%b),y", ADDR3_L);//        lda LKHI,x
    __asm__ ("sta %b", ADDR1_H);
    __asm__ ("lda %b", DATA1);
    __asm__ ("ldy %b", DATA2);          // column
    __asm__ ("sta (%b),y", ADDR1_L);
    __asm__ ("dex");
    __asm__ ("bne vl1");
}

// ; Draw or erase a ball
// ; BALL = ball number
// ; BALLYH+BALL = top row
// ; BALLXH+BALL = left column (byte offset)
// ; BALLXL+BALL = bits 4-6 select one of the 7 shifted versions
// ;
// ; Uses GBASL, GBASH, HGRY, HGRX
//
// ; draw ball 20us long
//
// xorball:
//         ldy BALL
//         lda BALLYH,y    ; Get row
//         sta HGRY
//         lda BALLXH,y    ; Get column
//         sta HGRX
//         lda BALLXL,y    ; Get Shift (0,8,...,48)
//         and #$38
//         tax             ; Offset into sprite table (pixel * 8)


void xorball(void)
{
    DATA1_P = BALL0[0];
    DATA2_P = BALL1[0];
}


static void hclear(void)
{
    pageset(HGR1SCRN_PAGE, BLACK, HGRSCRN_LENGTH);
    STROBE(HIRES);
    STROBE(TXTCLR);
}

static void hbox(void)
{
    hline(ROW_FIRST, WHITE);
    hline(ROW_LAST, WHITE);
    vline(COLUMN_FIRST, 0x03);
    vline(COLUMN_LAST, 0x60);
}

void main(void)
{

    hclear();
    hbox();

    TEST_PIN_TOGGLE;
    xorball();
    TEST_PIN_TOGGLE;

    while(1)
    {

    }
}
