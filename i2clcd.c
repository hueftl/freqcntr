#include "i2clcd.h"

// LCD Initialisierung
void lcd_init(void)
  {
    
  }

// Auf I2C schreiben
void lcd_write_i2c()
  {

  }

// Wartemöglichkeit
void lcd_wait_ms(unsigned short ms)
  {
	  unsigned short i;
	  for(i = 0; i < ms; i++)
	    {
		    wait1ms;
	    }
  }
