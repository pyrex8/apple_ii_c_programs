// make
// python3 ~/pico/pico-iie/main.py main.bin

#include <peekpoke.h>

int main(void)
{
    POKE(0x400, 0);
    return 0;
}
