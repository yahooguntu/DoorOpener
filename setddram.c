#include <p18cxxx.h>
#include "my_xlcd.h"

/********************************************************************
 *       Function Name:  SetDDRamAddr                                *
 *       Return Value:   void                                        *
 *       Parameters:     CGaddr: display data address                *
 *       Description:    This routine sets the display data address  *
 *                       of the Hitachi HD44780 LCD controller. The  *
 *                       user must check to see if the LCD controller*
 *                       is busy before calling this routine.        *
 ********************************************************************/
void SetDDRamAddr(unsigned char DDaddr) {
    OLATB_curr = (((DDaddr | 0b10000000) >> 3) & 0x1E) | (OLATB_curr & 0x01); // write upper nibble
    I2C_Write(EXPANDER_OLATB, OLATB_curr);

    DelayFor18TCY();
    clockLCDHigh(); // Clock the cmd and address in
    DelayFor18TCY();
    clockLCDLow();

    // Lower nibble interface
    OLATB_curr = ((DDaddr & 0x0F) << 1) | (OLATB_curr & 0x01); // Write lower nibble
    I2C_Write(EXPANDER_OLATB, OLATB_curr);

    DelayFor18TCY();
    clockLCDHigh(); // Clock the cmd and address in
    DelayFor18TCY();
    clockLCDLow();
    
    return;
}

