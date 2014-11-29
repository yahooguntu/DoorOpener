#include <p18cxxx.h>
#include "my_xlcd.h"

/********************************************************************
 *       Function Name:  BusyXLCD                                    *
 *       Return Value:   char: busy status of LCD controller         *
 *       Parameters:     void                                        *
 *       Description:    This routine reads the busy status of the   *
 *                       Hitachi HD44780 LCD controller.             *
 ********************************************************************/
unsigned char busy;

unsigned char BusyXLCD(void) {
    OLATB_curr |= 0xC0; // RW and RS are on
    clockLCDLow();
    I2C_Write(EXPANDER_IODIRB, 0x1E); // data bus is now an input
    I2C_Write(EXPANDER_OLATB, OLATB_curr);
    DelayFor18TCY();
    clockLCDHigh(); // Clock in the command with E
    DelayFor18TCY();
    
    busy = I2C_Read(EXPANDER_GPIOB);

    clockLCDLow(); // Reset clock line
    DelayFor18TCY();
    clockLCDHigh(); // Clock out other nibble
    DelayFor18TCY();
    clockLCDLow();
    OLATB_curr &= 0xBF; // Reset control line
    I2C_Write(EXPANDER_OLATB, OLATB_curr);
    I2C_Write(EXPANDER_IODIRB, 0x00); // set data bus back to output

    // busy bit is high?
    if (busy & 0x02)
    {
        return 1;
    }

    // Busy bit is low
    return 0;
}

