
// Note all timing is based on the slower emulated apple IIe that is taking
// approx 1.5us per cycle.

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"
#include "../zero_page/zero_page.h"

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

// divide-by 7 table
const uint8_t DIV7[256] =
{
     0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
     4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9,
     9, 9, 9, 9, 9, 9,10,10,10,10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,12,13,13,13,13,13,
    13,13,14,14,14,14,14,14,14,15,15,15,15,15,15,15,16,16,16,16,16,16,16,17,17,17,17,17,17,17,18,18,
    18,18,18,18,18,19,19,19,19,19,19,19,20,20,20,20,20,20,20,21,21,21,21,21,21,21,22,22,22,22,22,22,
    22,23,23,23,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,26,26,26,26,26,27,27,27,
    27,27,27,27,28,28,28,28,28,28,28,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31,31,31,
    32,32,32,32,32,32,32,33,33,33,33,33,33,33,34,34,34,34,34,34,34,35,35,35,35,35,35,35,36,36,36,36,
};

// modulo-by-7 table
const uint8_t MOD7[256] =
{
    0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3,
    4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0,
    1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4,
    5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1,
    2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5,
    6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2,
    3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6,
    0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3,
};

#define SPRITE_XH_CALC(x) (DIV7[x])
#define SPRITE_XL_CALC(x) (MOD7[x])

static const uint8_t sprites[] =
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,

    0x88, 0x80, 0x88, 0x80, 0x94, 0x80, 0x94, 0x80, 0x2A, 0x00, 0x2A, 0x00, 0x55, 0x00, 0x55, 0x00,
    0x90, 0x80, 0x90, 0x80, 0xA8, 0x80, 0xA8, 0x80, 0x54, 0x00, 0x54, 0x00, 0x2A, 0x01, 0x2A, 0x01,
    0xA0, 0x80, 0xA0, 0x80, 0xD0, 0x80, 0xD0, 0x80, 0x28, 0x01, 0x28, 0x01, 0x54, 0x02, 0x54, 0x02,
    0xC0, 0x80, 0xC0, 0x80, 0xA0, 0x81, 0xA0, 0x81, 0x50, 0x02, 0x50, 0x02, 0x28, 0x05, 0x28, 0x05,
    0x80, 0x81, 0x80, 0x81, 0xC0, 0x82, 0xC0, 0x82, 0x20, 0x05, 0x20, 0x05, 0x50, 0x0A, 0x50, 0x0A,
    0x80, 0x82, 0x80, 0x82, 0x80, 0x85, 0x80, 0x85, 0x40, 0x0A, 0x40, 0x0A, 0x20, 0x15, 0x20, 0x15,
    0x80, 0x84, 0x80, 0x84, 0x80, 0x8A, 0x80, 0x8A, 0x00, 0x15, 0x00, 0x15, 0x40, 0x2A, 0x40, 0x2A,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x88, 0x80, 0x88, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x90, 0x80, 0x90, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xA0, 0x80, 0xA0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x80, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x81, 0x80, 0x81, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x82, 0x80, 0x82, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x84, 0x80, 0x84, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x88, 0x80, 0x88, 0x80, 0x14, 0x00, 0x14, 0x00, 0x2A, 0x00, 0x2A, 0x00, 0xD5, 0x80, 0xD5, 0x80,
    0x90, 0x80, 0x90, 0x80, 0x28, 0x00, 0x28, 0x00, 0x54, 0x00, 0x54, 0x00, 0xAA, 0x81, 0xAA, 0x81,
    0xA0, 0x80, 0xA0, 0x80, 0x50, 0x00, 0x50, 0x00, 0x28, 0x01, 0x28, 0x01, 0xD4, 0x82, 0xD4, 0x82,
    0xC0, 0x80, 0xC0, 0x80, 0x20, 0x01, 0x20, 0x01, 0x50, 0x02, 0x50, 0x02, 0xA8, 0x85, 0xA8, 0x85,
    0x80, 0x81, 0x80, 0x81, 0x40, 0x02, 0x40, 0x02, 0x20, 0x05, 0x20, 0x05, 0xD0, 0x8A, 0xD0, 0x8A,
    0x80, 0x82, 0x80, 0x82, 0x00, 0x05, 0x00, 0x05, 0x40, 0x0A, 0x40, 0x0A, 0xA0, 0x95, 0xA0, 0x95,
    0x80, 0x84, 0x80, 0x84, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x15, 0x00, 0x15, 0xC0, 0xAA, 0xC0, 0xAA,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


static uint8_t sprite_x1;
static uint8_t sprite_y1;
static uint8_t sprite_x2;
static uint8_t sprite_y2;

static uint8_t sprite_xl;
static uint8_t sprite_xh;

static uint8_t sprite_no_jump;

static uint8_t sprite_buffer[SPRITE_BUFFER_SIZE];

union my_uint16_t
{
   uint16_t value;
   uint8_t b[2];
};

union my_uint16_t x16;


static void pointers_init(void)
{
    LKLOL_P = (uint8_t)lklo;
    LKLOH_P = (uint8_t)(((uint16_t)lklo)>> 8);
    LKHIL_P = (uint8_t)lkhi;
    LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> 8);
    SBUFRL_P = (uint8_t)sprite_buffer;
    SBUFRH_P = (uint8_t)(((uint16_t)sprite_buffer)>> 8);
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


static void sprite_hgr_to_buffer(uint8_t column, uint8_t row)
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

static void sprite_xor(uint8_t page, uint8_t sprite, uint8_t column, uint8_t row, uint8_t shift)
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

    SPRITEL_P = (uint8_t)sprites;
    SPRITEH_P = (uint8_t)(((uint16_t)sprites)>> 8);

    // init
    __asm__ ("lda %b", SPRITEL);
    __asm__ ("clc");
    __asm__ ("adc %b", SPRITE_NUM);
    __asm__ ("sta %b", SPRITEL);

    __asm__ ("lda %b", SPRITEH);
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

static void sprite_buffer_to_hgr(uint8_t column, uint8_t row)
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

void sprite_update(uint8_t page, uint8_t sprite1, uint8_t x1, uint8_t y1, uint8_t sprite2, uint8_t x2, uint8_t y2)
{
    // 9200us
    uint8_t col_delta = 1;
    uint8_t row_delta = 1 + y2 - y1;

    sprite_xl = SPRITE_XL_CALC(x1);
    sprite_xh = SPRITE_XH_CALC(x1);

    sprite_hgr_to_buffer(sprite_xh, y1);
    sprite_xor(page, sprite1, 1, 1, sprite_xl);

    sprite_xl = SPRITE_XL_CALC(x2);
    col_delta += SPRITE_XH_CALC(x2) - sprite_xh;

    sprite_xor(page, sprite2, col_delta, row_delta, sprite_xl);
    sprite_buffer_to_hgr(sprite_xh, y1);
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

void delay(void)
{
    uint8_t i = 0;
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
    for (i = 0; i < 200; i++)
    {
    }
}

void main(void)
{
    pointers_init();
    hclear();
    hbox();

    sprite_no_jump = 1;
    sprite_x1 = 140;
    sprite_y1 = 100;
    sprite_x2 = sprite_x1;
    sprite_y2 = sprite_y1;
    sprite_update(0, 0, sprite_x1, sprite_y1, 128, sprite_x2, sprite_y2);

    sprite_update(0, 0, 255, 150, 128, 255, 150);
    sprite_update(0, 0, 0, 150, 128, 0, 150);

    while(1)
    {
        sprite_x2 += 2;

        if (sprite_x2 > 240)
        {
            sprite_update(0, 128, sprite_x1, sprite_y1, 0, sprite_x2, sprite_y2);
            sprite_x2 = 10;
            sprite_x1 = sprite_x2;
            sprite_y1 = sprite_y2;
            sprite_update(0, 0, sprite_x1, sprite_y1, 128, sprite_x2, sprite_y2);
            sprite_no_jump = 0;
        }

        if (sprite_no_jump)
        {
            sprite_update(0, 128, sprite_x1, sprite_y1, 128, sprite_x2, sprite_y2);

            sprite_x1 = sprite_x2;
            sprite_y1 = sprite_y2;
        }
        sprite_no_jump = 1;


        delay();

        TEST_PIN_TOGGLE; // adds 2.5us
        hline(5, 5, 40, WHITE);
        TEST_PIN_TOGGLE; // adds 2.5us
        vline(10, 0, 192, 0x03);

    }
}
