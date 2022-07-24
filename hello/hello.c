
#include <stdio.h>
#include "../lib/test_pin.h"

void main(void)
{
   printf("HELLO, WORLD!\n");

   while(1)
   {
       TEST_PIN_TOGGLE;
       TEST_PIN_TOGGLE;
   }
}
