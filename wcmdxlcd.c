#include <p18cxxx.h>
#include "my_xlcd.h"

/********************************************************************
 *       Function Name:  WriteCmdXLCD                                *
 *       Return Value:   void                                        *
 *       Parameters:     cmd: command to send to LCD                 *
 *       Description:    This routine writes a command to the Hitachi*
 *                       HD44780 LCD controller. The user must check *
 *                       to see if the LCD controller is busy before *
 *                       calling this routine.                       *
 ********************************************************************/
void WriteCmdXLCD(unsigned char cmd) {
    // push first 4 bits in
    OLATB_curr = ((cmd << 1) & 0x1E) | (OLATB_curr & 0x01); // preserve blue LED
    // RW, RS, and E are all cleared
    I2C_Write(EXPANDER_OLATB, OLATB_curr);

    DelayFor18TCY();
    clockLCDHigh(); // Clock command in
    DelayFor18TCY();
    clockLCDLow();

    OLATB_curr = ((cmd >> 3) & 0x1E) | (OLATB_curr & 0x01); // preserve blue LED

    DelayFor18TCY();
    clockLCDHigh(); // Clock command in
    DelayFor18TCY();
    clockLCDLow();
    
    return;
}

