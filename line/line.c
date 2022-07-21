
// Note all timing is based on the slower emulated apple IIe that is taking
// approx 1.5us per cycle.

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define ROWS                192	// number of scanlines
#define ROW_FIRST           0
#define ROW_LAST            ROWS - 1
#define ROW_SECOND_LAST     ROW_LAST - 1
#define COLUMNS             40            // Number of columns/bytes per row
#define COLUMN_FIRST        0
#define COLUMN_LAST         COLUMNS - 1
#define WHITE               0x7F           // White hires byte
#define BLACK               0x00
#define SPRITE_BUFFER_SIZE  45
#define HGR1SCRN            0x2000
#define HGR_SCRN_LEN        0x2000

#define TXTCLR              0xC050         // graphics mode
#define HIRES               0xC057         // hires mode

// Addresses
enum Zero_page
{
    DATA1 = 0x26,
    DATA2,
    DATA3,
    DATA4,

    ADDR1L,
    ADDR1H,
    ADDR2L,
    ADDR2H,

    LKLOL,
    LKLOH,
    LKHIL,
    LHHIH,
};

// low byte is used for some instructions, alias so that it maps to assembly version
#define LKLO                LKLOL
#define LKHI                LKHIL
#define SPRITE              SPRITEL
#define SBUFR               SBUFRL // sprint buffer location

// pointers to zero page memory
#define DATA1_P             *((uint8_t*)DATA1)
#define DATA2_P             *((uint8_t*)DATA2)
#define DATA3_P             *((uint8_t*)DATA3)
#define DATA4_P             *((uint8_t*)DATA4)

#define ADDR1L_P            *((uint8_t*)ADDR1L)
#define ADDR1H_P            *((uint8_t*)ADDR1H)
#define ADDR2L_P            *((uint8_t*)ADDR2L)
#define ADDR2H_P            *((uint8_t*)ADDR2H)

#define LKLOL_P             *((uint8_t*)LKLOL)
#define LKLOH_P             *((uint8_t*)LKLOH)
#define LKHIL_P             *((uint8_t*)LKHIL)
#define LKHIH_P             *((uint8_t*)LHHIH)


static const uint8_t lklo[256] =
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

    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

static const uint8_t lkhi[256] =
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

    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

static void pointers_init(void)
{
    LKLOL_P = (uint8_t)lklo;
    LKLOH_P = (uint8_t)(((uint16_t)lklo)>> 8);
    LKHIL_P = (uint8_t)lkhi;
    LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> 8);
}

static void hline(uint8_t column, uint8_t row, uint8_t length, uint8_t pixels)
{
    // 1.7ms
    DATA1_P = pixels;
    DATA2_P = column;
    DATA3_P = column + length;
    ADDR1L_P = lklo[row];
    ADDR1H_P = lkhi[row];

    // init
    __asm__ ("ldy %b", DATA3);

    // loop
    __asm__ ("hl1: dey");
    __asm__ ("lda (%b),y", ADDR1L);
    __asm__ ("eor %b", DATA1);
    __asm__ ("sta (%b),y", ADDR1L);
    __asm__ ("cpy %b", DATA2);
    __asm__ ("bne hl1");
}

static void vline(uint8_t column, uint8_t row, uint8_t length, uint8_t pixels)
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

static void hclear(void)
{
    // 77.9ms
    memset((uint8_t *)HGR1SCRN, 0, HGR_SCRN_LEN); // clear page 1
    STROBE(HIRES);
    STROBE(TXTCLR);
}

static void hbox(void)
{
    hline(COLUMN_FIRST, ROW_FIRST, COLUMNS, WHITE);
    hline(COLUMN_FIRST, ROW_LAST, COLUMNS, WHITE);
    vline(COLUMN_FIRST, 0, 192, 0x03);
    vline(COLUMN_LAST, 0, 192, 0x60);
}

void main(void)
{
    pointers_init();
    hclear();
    hbox();

    while(1)
    {
    }
}
