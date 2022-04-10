
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define BYTE_FULL       0xFF
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
    ADDR3L,
    ADDR3H,

    GBASL,
    GBASH,

    LKLOL,
    LKLOH,
    LKHIL,
    LHHIH,

    BALL0L,
    BALL0H,
    BALL1L,
    BALL1H,

    TEST,
};

// low byte is used for some instructions, alias so that it maps to assembly version
#define LKLO  LKLOL
#define LKHI  LKHIL
#define BALL0  BALL0L
#define BALL1  BALL1L

// pointers to zero page memory
#define DATA1_P         *((uint8_t*)DATA1)
#define DATA2_P         *((uint8_t*)DATA2)
#define DATA3_P         *((uint8_t*)DATA3)
#define DATA4_P         *((uint8_t*)DATA4)
#define DATA5_P         *((uint8_t*)DATA5)
#define ADDR1L_P        *((uint8_t*)ADDR1L)
#define ADDR1H_P        *((uint8_t*)ADDR1H)
#define ADDR2L_P        *((uint8_t*)ADDR2L)
#define ADDR2H_P        *((uint8_t*)ADDR2H)
#define ADDR3L_P        *((uint8_t*)ADDR3L)
#define ADDR3H_P        *((uint8_t*)ADDR3H)

#define GBASL_P         *((uint8_t*)GBASL)
#define GBASH_P         *((uint8_t*)GBASH)

#define LKLOL_P         *((uint8_t*)LKLOL)
#define LKLOH_P         *((uint8_t*)LKLOH)
#define LKHIL_P         *((uint8_t*)LKHIL)
#define LKHIH_P         *((uint8_t*)LHHIH)

#define BALL0L_P         *((uint8_t*)BALL0L)
#define BALL0H_P         *((uint8_t*)BALL0H)
#define BALL1L_P         *((uint8_t*)BALL1L)
#define BALL1H_P         *((uint8_t*)BALL1H)

#define TEST_P         *((uint8_t*)TEST)

#define HCOLOR1         0x1C           // Color value

#define HGR1SCRN        0x2000         // Start of hires page 1
#define HGR1SCRN_PAGE   0x20
#define HGRSCRN_LENGTH  0x20        // number of pages

#define KBD             0xC000         // key code when MSB set
#define KBDSTRB         0xC010         // clear keyboard buffer
#define TXTCLR          0xC050         // graphics mode
#define TXTSET          0xC051         // text mode
#define LOWSCR          0xC054         // page 1
#define HIRES           0xC057         // hires mode

static uint8_t ballxl[] = {0x00, 0x00};
static uint8_t ballxh[] = {0x00, 0x00};

static uint8_t ballyl[] = {0x00, 0x00};
static uint8_t ballyh[] = {0x00, 0x00};

static uint8_t balldyl[] = {0x00, 0x00};
static uint8_t balldyh[] = {0x00, 0x00};

static uint8_t balldx[] = {0x00, 0x00};

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

static const uint8_t ball0[] =
{
    0X3C, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X3C,
    0X78, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X78,
    0X70, 0X7C, 0X7C, 0X7C, 0X7C, 0X7C, 0X7C, 0X70,
    0X60, 0X78, 0X78, 0X78, 0X78, 0X78, 0X78, 0X60,
    0X40, 0X70, 0X70, 0X70, 0X70, 0X70, 0X70, 0X40,
    0X00, 0X60, 0X60, 0X60, 0X60, 0X60, 0X60, 0X00,
    0X00, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X00,
};

static const uint8_t ball1[] =
{
    0X00, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X00,
    0X00, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X00,
    0X01, 0X07, 0X07, 0X07, 0X07, 0X07, 0X07, 0X01,
    0X03, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X03,
    0X07, 0X1F, 0X1F, 0X1F, 0X1F, 0X1F, 0X1F, 0X07,
    0X0F, 0X3F, 0X3F, 0X3F, 0X3F, 0X3F, 0X3F, 0X0F,
    0X1E, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X1E,
};

static void pointers_init(void)
{
    LKLOL_P = (uint8_t)lklo;
    LKLOH_P = (uint8_t)(((uint16_t)lklo)>> 8);
    LKHIL_P = (uint8_t)lkhi;
    LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> 8);

    BALL0L_P = (uint8_t)ball0;
    BALL0H_P = (uint8_t)(((uint16_t)ball0)>> 8);
    BALL1L_P = (uint8_t)ball1;
    BALL1H_P = (uint8_t)(((uint16_t)ball1) >> 8);
}

static void pageset(uint8_t page, uint8_t value, uint8_t length)
{
    // this method takes 114ms
    DATA1_P = value;
    DATA2_P = length;
    ADDR1L_P = 0x00;
    ADDR1H_P = page;

    // init registers with memory
    __asm__ ("lda %b", DATA1);    // value to fill page(s) with
    __asm__ ("ldx %b", DATA2);    // number of pages
    __asm__ ("ldy %b", ADDR1L); // address two bytes

    // nested loops
    __asm__ ("hclr1: sta (%b),y", ADDR1L);
    __asm__ ("iny");
    __asm__ ("bne hclr1");
    __asm__ ("inc %b", ADDR1H);
    __asm__ ("dex");
    __asm__ ("bne hclr1");
}

static void hline(uint8_t line, uint8_t pixels)
{
    // Assembly = 580us, C = 850us
    DATA1_P = pixels;
    ADDR1L_P = lklo[line];
    ADDR1H_P = lkhi[line];

    // init
    __asm__ ("ldy #%b", COLUMN_LAST); // Width of screen in bytes
    __asm__ ("lda %b", DATA1);

    // loop
    __asm__ ("hl1: sta (%b),y", ADDR1L);
    __asm__ ("dey");
    __asm__ ("bpl hl1");
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
    __asm__ ("lda (%b),y", LKHI);//        lda lkhi,x
    __asm__ ("sta %b", ADDR1H);
    __asm__ ("lda %b", DATA1);
    __asm__ ("ldy %b", DATA2);          // column
    __asm__ ("sta (%b),y", ADDR1L);
    __asm__ ("dex");
    __asm__ ("bne vl1");
}

void xorball(uint8_t ball)
{
    #define BALL DATA1
    #define HGRY DATA2
    #define HGRX DATA3
    #define ROW  DATA4
    #define BALL_INDEX  DATA5

    #define BALLYH ADDR1L
    #define BALLXH ADDR2L
    #define BALLXL ADDR3L

    // Assembly = 720us, C = 1110us
    DATA1_P = ball;
    ADDR1L_P = (uint8_t)ballyh;
    ADDR1H_P = (uint8_t)(((uint16_t)ballyh)>> 8);
    ADDR2L_P = (uint8_t)ballxh;
    ADDR2H_P = (uint8_t)(((uint16_t)ballxh)>> 8);
    ADDR3L_P = (uint8_t)ballxl;
    ADDR3H_P = (uint8_t)(((uint16_t)ballxl)>> 8);
    TEST_P = BYTE_FULL;

    // init
    __asm__ ("ldy %b", BALL);
    __asm__ ("lda (%b), y", BALLYH); // Get row
    __asm__ ("sta %b", HGRY);
    __asm__ ("lda (%b), y", BALLXH); // Get column
    __asm__ ("sta %b", HGRX);
    __asm__ ("lda (%b), y", BALLXL); // Get Shift (0,8,...,48)
    __asm__ ("and #%b", 0x38);
    __asm__ ("tax");
    __asm__ ("sta %b", BALL_INDEX); // Offset into sprite table (pixel * 8)

// (GBASL, GBASH) = row address
// Y = byte offset into the row
// X = index into sprite tables

    // loop
    __asm__ ("xsplot: ldy %b", HGRY); // Get the row address
    __asm__ ("lda (%b), y", LKLO);
    __asm__ ("sta %b", GBASL);
    __asm__ ("lda (%b), y", LKHI);
    __asm__ ("sta %b", GBASH);
    __asm__ ("iny");
    __asm__ ("sty %b", HGRY);

    __asm__ ("ldy %b", HGRX);
    __asm__ ("lda (%b),y", GBASL);

    __asm__ ("ldy %b", BALL_INDEX);
    __asm__ ("eor (%b), y", BALL0); // XOR the two bytes onto the screen

    __asm__ ("ldy %b", HGRX);
    __asm__ ("sta (%b),y", GBASL);

    __asm__ ("iny");
    __asm__ ("lda (%b),y", GBASL);

    __asm__ ("ldy %b", BALL_INDEX);
    __asm__ ("eor (%b), y", BALL1);

    __asm__ ("iny");
    __asm__ ("sta (%b),y", GBASL);


    __asm__ ("inx");
    __asm__ ("txa");
    __asm__ ("and #%b", 7);
    __asm__ ("bne xsplot"); // Stop at a multiple of 8 bytes
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
    ballxl[0] = 0;
    ballxh[0] = 10;

    ballyl[0] = 0;
    ballyh[0] = 10;

    balldyl[0] = 0;
    balldyh[0] = 0;

    balldx[0] = 0;

    pointers_init();
    hclear();
    hbox();

    TEST_PIN_TOGGLE;
    xorball(0);
    TEST_PIN_TOGGLE;

    while(1)
    {

    }
}
