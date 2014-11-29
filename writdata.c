#include <p18cxxx.h>
#include "my_xlcd.h"

/********************************************************************
 *       Function Name:  WriteDataXLCD                               *
 *       Return Value:   void                                        *
 *       Parameters:     data: data byte to be written to LCD        *
 *       Description:    This routine writes a data byte to the      *
 *                       Hitachi HD44780 LCD controller. The user    *
 *                       must check to see if the LCD controller is  *
 *                       busy before calling this routine. The data  *
 *                       is written to the character generator RAM or*
 *                       the display data RAM depending on what the  *
 *                       previous SetxxRamAddr routine was called.   *
 ********************************************************************/
void WriteDataXLCD(char data) {
    data = reverseNibbles(data);

    // 4-bit interface
    OLATB_curr = ((data << 1) & 0x1E) | (OLATB_curr & 0x01);
    OLATB_curr |= 0x80; // set RS
    I2C_Write(EXPANDER_OLATB, OLATB_curr);

    DelayFor18TCY();
    clockLCDHigh(); // Clock nibble into LCD
    DelayFor18TCY();
    clockLCDLow();

    // Lower nibble interface
    OLATB_curr &= 0xE1; // clear data bits
    OLATB_curr |= (data >> 3) & 0x1E; // or in lower 4 data bits

    DelayFor18TCY();
    clockLCDHigh(); // Clock nibble into LCD
    DelayFor18TCY();
    clockLCDLow();
    
    return;
}

