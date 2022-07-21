



#define ROWS                192

enum Zero_page
{
    DATA1 = 0x26,
    DATA2,
    DATA3,
    DATA4,
    DATA5,
};


union my_uint16_t
{
   uint16_t value;
   uint8_t b[2];
};

union my_uint16_t x16;


// 8 bit for loop
for (i = 0; i < 200; i++)
{
}

// 16 bit nested for loop
for (i = 0; i < 200; i++)
{
    for (j = 0; j < 200; j++)
    {

    }
}


if (value)
{

}

if (value1 == value2)
{

}

if (value1 != value2)
{

}

if (value1 > LIMIT)
{

}

value1 = value2;
value1 = value2 + value3;
value1++;
value1--;
value1 += 2;
value1 -= 2;

value |= 0x0F;
value &= 0x0F;

value1 = value2>>1;
value1 = value2<<1;

#include <stdio.h>

void main(void)
{
    printf("HELLO, 6502!\n");
}


#include <string.h>

void main(void)
{
    memset((uint8_t*)0x2000, 0, 0x2000); // clear page 1
}


#include <stdint.h>

uint8_t value1 = 8;

void main(void)
{
}



uint8_t value1 = 8;

void main(void)
{
    while(1)
    {
    }
}



printf();
memcpy();
memset();


// macros
PEEK();
POKE();
STROBE();

#define JOYSTICK_FIRE (PEEK(JOYSTICK_BUTTON0_ADDR) > 127 ? 1 : 0)

const uint8_t BIT7[7] = { 1, 2, 4, 8, 16, 32, 64 };
