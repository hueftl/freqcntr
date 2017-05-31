#include "i2clcd.h"

void lcd_init(void)
{

}

void lcd_write_i2c()
{

}

void lcd_wait_ms(unsigned short ms)
{
    unsigned short i;
    for(i = 0; i < ms; i++) {
        wait1ms;
    }
}
