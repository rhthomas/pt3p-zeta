#include "led.h"

void led_set(uint8_t byte)
{
    PJOUT = (byte & 0x0F);
    P3OUT = (byte & 0xF0);
}

void led_clear(void)
{
    PJOUT = 0;
    P3OUT = 0;
}
