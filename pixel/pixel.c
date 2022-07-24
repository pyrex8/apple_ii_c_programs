
#include <stdio.h>
#include <stdint.h>
#include "../lib/test_pin.h"

#include <string.h>

/// HIRES LOOKUP TABLE

#define VHEIGHT 192	// number of scanlines
#define PIXELS 11
#define PIXELS_TOTAL 11

// put pixels in array
// save old location, old mask
// array of x and y, and color
// array of change pixel (limited)
// calcluate new location, new mask
// interate throught old - new pixel (xor pixel function)

static uint8_t pixel_x[PIXELS_TOTAL];
static uint8_t pixel_y[PIXELS_TOTAL];
//static uint8_t pixel_color[PIXELS_TOTAL];
//static uint8_t pixel_new[PIXELS_TOTAL];
static uint8_t pixel_mask_old[PIXELS_TOTAL];
static uint8_t pixel_mask_new[PIXELS_TOTAL];
static uint16_t pixel_addr_old[PIXELS_TOTAL];
static uint16_t pixel_addr_new[PIXELS_TOTAL];

uint8_t pixel;
uint8_t mask;
uint8_t value;
uint16_t addr;


// starting address of each scanline
static const uint16_t vidline[] =
{
  0X2000, 0X2400, 0X2800, 0X2C00, 0X3000, 0X3400, 0X3800, 0X3C00,
  0X2080, 0X2480, 0X2880, 0X2C80, 0X3080, 0X3480, 0X3880, 0X3C80,
  0X2100, 0X2500, 0X2900, 0X2D00, 0X3100, 0X3500, 0X3900, 0X3D00,
  0X2180, 0X2580, 0X2980, 0X2D80, 0X3180, 0X3580, 0X3980, 0X3D80,
  0X2200, 0X2600, 0X2A00, 0X2E00, 0X3200, 0X3600, 0X3A00, 0X3E00,
  0X2280, 0X2680, 0X2A80, 0X2E80, 0X3280, 0X3680, 0X3A80, 0X3E80,
  0X2300, 0X2700, 0X2B00, 0X2F00, 0X3300, 0X3700, 0X3B00, 0X3F00,
  0X2380, 0X2780, 0X2B80, 0X2F80, 0X3380, 0X3780, 0X3B80, 0X3F80,
  0X2028, 0X2428, 0X2828, 0X2C28, 0X3028, 0X3428, 0X3828, 0X3C28,
  0X20A8, 0X24A8, 0X28A8, 0X2CA8, 0X30A8, 0X34A8, 0X38A8, 0X3CA8,
  0X2128, 0X2528, 0X2928, 0X2D28, 0X3128, 0X3528, 0X3928, 0X3D28,
  0X21A8, 0X25A8, 0X29A8, 0X2DA8, 0X31A8, 0X35A8, 0X39A8, 0X3DA8,
  0X2228, 0X2628, 0X2A28, 0X2E28, 0X3228, 0X3628, 0X3A28, 0X3E28,
  0X22A8, 0X26A8, 0X2AA8, 0X2EA8, 0X32A8, 0X36A8, 0X3AA8, 0X3EA8,
  0X2328, 0X2728, 0X2B28, 0X2F28, 0X3328, 0X3728, 0X3B28, 0X3F28,
  0X23A8, 0X27A8, 0X2BA8, 0X2FA8, 0X33A8, 0X37A8, 0X3BA8, 0X3FA8,
  0X2050, 0X2450, 0X2850, 0X2C50, 0X3050, 0X3450, 0X3850, 0X3C50,
  0X20D0, 0X24D0, 0X28D0, 0X2CD0, 0X30D0, 0X34D0, 0X38D0, 0X3CD0,
  0X2150, 0X2550, 0X2950, 0X2D50, 0X3150, 0X3550, 0X3950, 0X3D50,
  0X21D0, 0X25D0, 0X29D0, 0X2DD0, 0X31D0, 0X35D0, 0X39D0, 0X3DD0,
  0X2250, 0X2650, 0X2A50, 0X2E50, 0X3250, 0X3650, 0X3A50, 0X3E50,
  0X22D0, 0X26D0, 0X2AD0, 0X2ED0, 0X32D0, 0X36D0, 0X3AD0, 0X3ED0,
  0X2350, 0X2750, 0X2B50, 0X2F50, 0X3350, 0X3750, 0X3B50, 0X3F50,
  0X23D0, 0X27D0, 0X2BD0, 0X2FD0, 0X33D0, 0X37D0, 0X3BD0, 0X3FD0,
};


// divide-by 7 table
const uint8_t DIV7[256] = {
 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9,
 9, 9, 9, 9, 9, 9,10,10,10,10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,12,13,13,13,13,13,
13,13,14,14,14,14,14,14,14,15,15,15,15,15,15,15,16,16,16,16,16,16,16,17,17,17,17,17,17,17,18,18,
18,18,18,18,18,19,19,19,19,19,19,19,20,20,20,20,20,20,20,21,21,21,21,21,21,21,22,22,22,22,22,22,
22,23,23,23,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,26,26,26,26,26,27,27,27,
27,27,27,27,28,28,28,28,28,28,28,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31,31,31,
32,32,32,32,32,32,32,33,33,33,33,33,33,33,34,34,34,34,34,34,34,35,35,35,35,35,35,35,36,36,36,36};

// modulo-by-7 table
const uint8_t MOD7[256] = {
 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3,
 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0,
 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4,
 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1,
 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5,
 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2,
 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6,
 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3};

// bitmask table
const uint8_t BIT7[7] = { 1, 2, 4, 8, 16, 32, 64 };


#define PIXEL_ADDRESS(x, y) (vidline[y] + DIV7[x])
#define PIXEL_MASK(x) (BIT7[MOD7[x]])

/// GRAPHICS FUNCTIONS

// clear screen and set graphics mode
void clrscr() {
  STROBE(0xc052); // turn off mixed-mode
  STROBE(0xc054); // page 1
  STROBE(0xc057); // hi-res
  STROBE(0xc050); // set graphics mode
  memset((uint8_t*)0x2000, 0, 0x2000); // clear page 1
}


void pixel_calc_update(void)
{
    for (pixel = 0; pixel < PIXELS_TOTAL; pixel++)
    {
        pixel_addr_old[pixel] = pixel_addr_new[pixel];
        pixel_addr_new[pixel] = PIXEL_ADDRESS(pixel_x[pixel], pixel_y[pixel]);
        pixel_mask_old[pixel] = pixel_mask_new[pixel];
        pixel_mask_new[pixel] = PIXEL_MASK(pixel_x[pixel]);
    }
}

void pixel_display_update(void)
{
    for (pixel = 0; pixel < PIXELS_TOTAL; pixel++)
    {
        value = PEEK(pixel_addr_old[pixel]);
        value ^= pixel_mask_old[pixel];
        POKE(pixel_addr_old[pixel],value);
        value = PEEK(pixel_addr_new[pixel]);
        value ^= pixel_mask_new[pixel];
        POKE(pixel_addr_new[pixel],value);
    }
}

void main(void)
{
    uint8_t i = 0;
    uint8_t x = 0;
    clrscr();

    while(1)
    {
        TEST_PIN_TOGGLE;
        x += 4;
        for(i = 0; i < PIXELS_TOTAL; i++)
        {
            pixel_x[i] = x;
            pixel_y[i] = 50 + i;
        }
        pixel_calc_update();
        pixel_display_update();
   }
}
