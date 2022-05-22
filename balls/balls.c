
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
    DATA5,

    ADDR1L,
    ADDR1H,
    ADDR2L,
    ADDR2H,

    LKLOL,
    LKLOH,
    LKHIL,
    LHHIH,

    SBUFRL,
    SBUFRH,

    SPRITEL,
    SPRITEH,
};

// low byte is used for some instructions, alias so that it maps to assembly version
#define LKLO                LKLOL
#define LKHI                LKHIL
#define SPRITE               SPRITEL
#define SBUFR               SBUFRL // sprint buffer location

// pointers to zero page memory
#define DATA1_P             *((uint8_t*)DATA1)
#define DATA2_P             *((uint8_t*)DATA2)
#define DATA3_P             *((uint8_t*)DATA3)
#define DATA4_P             *((uint8_t*)DATA4)
#define DATA5_P             *((uint8_t*)DATA5)

#define ADDR1L_P            *((uint8_t*)ADDR1L)
#define ADDR1H_P            *((uint8_t*)ADDR1H)
#define ADDR2L_P            *((uint8_t*)ADDR2L)
#define ADDR2H_P            *((uint8_t*)ADDR2H)

#define LKLOL_P             *((uint8_t*)LKLOL)
#define LKLOH_P             *((uint8_t*)LKLOH)
#define LKHIL_P             *((uint8_t*)LKHIL)
#define LKHIH_P             *((uint8_t*)LHHIH)

#define SBUFRL_P            *((uint8_t*)SBUFRL)
#define SBUFRH_P            *((uint8_t*)SBUFRH)

#define SPRITEL_P            *((uint8_t*)SPRITEL)
#define SPRITEH_P            *((uint8_t*)SPRITEH)

static const uint8_t lklo[ROWS] =
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

static const uint8_t lkhi[ROWS] =
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

    0x10, 0x00, 0x64, 0x00, 0x02, 0x01, 0x42, 0x00, 0x01, 0x01, 0x46, 0x00, 0x68, 0x00, 0x10, 0x00,
    0x20, 0x00, 0x48, 0x01, 0x04, 0x02, 0x04, 0x01, 0x02, 0x02, 0x0C, 0x01, 0x50, 0x01, 0x20, 0x00,
    0x40, 0x00, 0x10, 0x03, 0x08, 0x04, 0x08, 0x02, 0x04, 0x04, 0x18, 0x02, 0x20, 0x03, 0x40, 0x00,
    0x00, 0x01, 0x20, 0x06, 0x10, 0x08, 0x10, 0x04, 0x08, 0x08, 0x30, 0x04, 0x40, 0x06, 0x00, 0x01,
    0x00, 0x02, 0x40, 0x0C, 0x20, 0x10, 0x20, 0x08, 0x10, 0x10, 0x60, 0x08, 0x00, 0x0D, 0x00, 0x02,
    0x00, 0x04, 0x00, 0x19, 0x40, 0x20, 0x40, 0x10, 0x20, 0x20, 0x40, 0x11, 0x00, 0x1A, 0x00, 0x04,
    0x00, 0x08, 0x00, 0x32, 0x00, 0x41, 0x00, 0x21, 0x40, 0x40, 0x00, 0x23, 0x00, 0x34, 0x00, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,

    0x10, 0x00, 0x64, 0x00, 0x02, 0x01, 0x42, 0x00, 0x01, 0x01, 0x46, 0x00, 0x68, 0x00, 0x10, 0x00,
    0x20, 0x00, 0x48, 0x01, 0x04, 0x02, 0x04, 0x01, 0x02, 0x02, 0x0C, 0x01, 0x50, 0x01, 0x20, 0x00,
    0x40, 0x00, 0x10, 0x03, 0x08, 0x04, 0x08, 0x02, 0x04, 0x04, 0x18, 0x02, 0x20, 0x03, 0x40, 0x00,
    0x00, 0x01, 0x20, 0x06, 0x10, 0x08, 0x10, 0x04, 0x08, 0x08, 0x30, 0x04, 0x40, 0x06, 0x00, 0x01,
    0x00, 0x02, 0x40, 0x0C, 0x20, 0x10, 0x20, 0x08, 0x10, 0x10, 0x60, 0x08, 0x00, 0x0D, 0x00, 0x02,
    0x00, 0x04, 0x00, 0x19, 0x40, 0x20, 0x40, 0x10, 0x20, 0x20, 0x40, 0x11, 0x00, 0x1A, 0x00, 0x04,
    0x00, 0x08, 0x00, 0x32, 0x00, 0x41, 0x00, 0x21, 0x40, 0x40, 0x00, 0x23, 0x00, 0x34, 0x00, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#define LUT(x) (uint8_t*)(0x2000|x)

// starting address of each scanline
static uint8_t* const vidmem[ROWS] = {
  LUT(0x0000), LUT(0x0400), LUT(0x0800), LUT(0x0c00), LUT(0x1000), LUT(0x1400), LUT(0x1800), LUT(0x1c00),
  LUT(0x0080), LUT(0x0480), LUT(0x0880), LUT(0x0c80), LUT(0x1080), LUT(0x1480), LUT(0x1880), LUT(0x1c80),
  LUT(0x0100), LUT(0x0500), LUT(0x0900), LUT(0x0d00), LUT(0x1100), LUT(0x1500), LUT(0x1900), LUT(0x1d00),
  LUT(0x0180), LUT(0x0580), LUT(0x0980), LUT(0x0d80), LUT(0x1180), LUT(0x1580), LUT(0x1980), LUT(0x1d80),
  LUT(0x0200), LUT(0x0600), LUT(0x0a00), LUT(0x0e00), LUT(0x1200), LUT(0x1600), LUT(0x1a00), LUT(0x1e00),
  LUT(0x0280), LUT(0x0680), LUT(0x0a80), LUT(0x0e80), LUT(0x1280), LUT(0x1680), LUT(0x1a80), LUT(0x1e80),
  LUT(0x0300), LUT(0x0700), LUT(0x0b00), LUT(0x0f00), LUT(0x1300), LUT(0x1700), LUT(0x1b00), LUT(0x1f00),
  LUT(0x0380), LUT(0x0780), LUT(0x0b80), LUT(0x0f80), LUT(0x1380), LUT(0x1780), LUT(0x1b80), LUT(0x1f80),
  LUT(0x0028), LUT(0x0428), LUT(0x0828), LUT(0x0c28), LUT(0x1028), LUT(0x1428), LUT(0x1828), LUT(0x1c28),
  LUT(0x00a8), LUT(0x04a8), LUT(0x08a8), LUT(0x0ca8), LUT(0x10a8), LUT(0x14a8), LUT(0x18a8), LUT(0x1ca8),
  LUT(0x0128), LUT(0x0528), LUT(0x0928), LUT(0x0d28), LUT(0x1128), LUT(0x1528), LUT(0x1928), LUT(0x1d28),
  LUT(0x01a8), LUT(0x05a8), LUT(0x09a8), LUT(0x0da8), LUT(0x11a8), LUT(0x15a8), LUT(0x19a8), LUT(0x1da8),
  LUT(0x0228), LUT(0x0628), LUT(0x0a28), LUT(0x0e28), LUT(0x1228), LUT(0x1628), LUT(0x1a28), LUT(0x1e28),
  LUT(0x02a8), LUT(0x06a8), LUT(0x0aa8), LUT(0x0ea8), LUT(0x12a8), LUT(0x16a8), LUT(0x1aa8), LUT(0x1ea8),
  LUT(0x0328), LUT(0x0728), LUT(0x0b28), LUT(0x0f28), LUT(0x1328), LUT(0x1728), LUT(0x1b28), LUT(0x1f28),
  LUT(0x03a8), LUT(0x07a8), LUT(0x0ba8), LUT(0x0fa8), LUT(0x13a8), LUT(0x17a8), LUT(0x1ba8), LUT(0x1fa8),
  LUT(0x0050), LUT(0x0450), LUT(0x0850), LUT(0x0c50), LUT(0x1050), LUT(0x1450), LUT(0x1850), LUT(0x1c50),
  LUT(0x00d0), LUT(0x04d0), LUT(0x08d0), LUT(0x0cd0), LUT(0x10d0), LUT(0x14d0), LUT(0x18d0), LUT(0x1cd0),
  LUT(0x0150), LUT(0x0550), LUT(0x0950), LUT(0x0d50), LUT(0x1150), LUT(0x1550), LUT(0x1950), LUT(0x1d50),
  LUT(0x01d0), LUT(0x05d0), LUT(0x09d0), LUT(0x0dd0), LUT(0x11d0), LUT(0x15d0), LUT(0x19d0), LUT(0x1dd0),
  LUT(0x0250), LUT(0x0650), LUT(0x0a50), LUT(0x0e50), LUT(0x1250), LUT(0x1650), LUT(0x1a50), LUT(0x1e50),
  LUT(0x02d0), LUT(0x06d0), LUT(0x0ad0), LUT(0x0ed0), LUT(0x12d0), LUT(0x16d0), LUT(0x1ad0), LUT(0x1ed0),
  LUT(0x0350), LUT(0x0750), LUT(0x0b50), LUT(0x0f50), LUT(0x1350), LUT(0x1750), LUT(0x1b50), LUT(0x1f50),
  LUT(0x03d0), LUT(0x07d0), LUT(0x0bd0), LUT(0x0fd0), LUT(0x13d0), LUT(0x17d0), LUT(0x1bd0), LUT(0x1fd0)
};

static uint8_t sprite_x1;
static uint8_t sprite_y1;
static uint8_t sprite_x2;
static uint8_t sprite_y2;

static uint8_t sprite_xl;
static uint8_t sprite_xh;

static uint8_t sprite_no_jump;

static uint8_t sprite_buffer[SPRITE_BUFFER_SIZE];

static void pointers_init(void)
{
    LKLOL_P = (uint8_t)lklo;
    LKLOH_P = (uint8_t)(((uint16_t)lklo)>> 8);
    LKHIL_P = (uint8_t)lkhi;
    LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> 8);
    SBUFRL_P = (uint8_t)sprite_buffer;
    SBUFRH_P = (uint8_t)(((uint16_t)sprite_buffer)>> 8);
}

static void hline(uint8_t line, uint8_t pixels)
{
    // 1.01ms
    memset(vidmem[line], pixels, 40);
}

static void vline(uint8_t column, uint8_t pixels)
{
    // Assembly = 6700us, C = 9400us
    DATA1_P = pixels;
    DATA2_P = column;
    DATA3_P = ROW_SECOND_LAST;

    // init
    __asm__ ("ldx %b", DATA3);          // Start at second-to-last row

    // loop
    __asm__ ("vl1: txa");               // row to a
    __asm__ ("tay");                    // row to y
    __asm__ ("lda (%b),y", LKLO);       // Get the row address
    __asm__ ("sta %b", ADDR1L);
    __asm__ ("lda (%b),y", LKHI);
    __asm__ ("sta %b", ADDR1H);
    __asm__ ("lda %b", DATA1);
    __asm__ ("ldy %b", DATA2);          // column
    __asm__ ("sta (%b),y", ADDR1L);
    __asm__ ("dex");
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
    hline(ROW_FIRST, WHITE);
    hline(ROW_LAST, WHITE);
    vline(COLUMN_FIRST, 0x03);
    vline(COLUMN_LAST, 0x60);
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

    while(1)
    {
        sprite_x2 += 2;
        sprite_y2 += 1;
        if (sprite_y2 > 180)
        {
            sprite_update(0, 128, sprite_x1, sprite_y1, 0, sprite_x2, sprite_y2);
            sprite_y2 = 10;
            sprite_x1 = sprite_x2;
            sprite_y1 = sprite_y2;
            sprite_update(0, 0, sprite_x1, sprite_y1, 128, sprite_x2, sprite_y2);
            sprite_no_jump = 0;
        }

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
        hline(ROW_FIRST, WHITE); // 1.3ms
        TEST_PIN_TOGGLE; // adds 2.5us
    }
}
