#include <p18cxxx.h>
#include "delays.h"
#include "my_xlcd.h"

/********************************************************************
 *       Function Name:  OpenXLCD                                    *
 *       Return Value:   void                                        *
 *       Parameters:     lcdtype: sets the type of LCD (lines)       *
 *       Description:    This routine configures the LCD. Based on   *
 *                       the Hitachi HD44780 LCD controller. The     *
 *                       routine will configure the I/O pins of the  *
 *                       microcontroller, setup the LCD for 4- or    *
 *                       8-bit mode and clear the display. The user  *
 *                       must provide three delay routines:          *
 *                       DelayFor18TCY() provides a 18 Tcy delay     *
 *                       DelayPORXLCD() provides at least 15ms delay *
 *                       DelayXLCD() provides at least 5ms delay     *
 ********************************************************************/
void OpenXLCD(unsigned char lcdtype) {
    // set up the MCP23017
    I2C_Write(EXPANDER_IODIRA, 0x1F);
    I2C_Write(EXPANDER_IODIRB, 0x00);

    // reset lines, white backlight (RGB on)
    OLATA_curr = 0xE0;
    OLATB_curr = 0x00; // not sure why F...
    I2C_Write(EXPANDER_OLATA, OLATA_curr);
    I2C_Write(EXPANDER_OLATB, OLATB_curr);

    // Delay for 15ms to allow for LCD Power on reset
    DelayPORXLCD();

    //-------------------reset procedure through software----------------------
    WriteCmdXLCD(0x0C);
    Delay10KTCYx(0x05);

    WriteCmdXLCD(0x0C);
    Delay10KTCYx(0x01);


    WriteCmdXLCD(0x0C);
    while (BusyXLCD());
    //------------------------------------------------------------------------------------------


    // Set data interface width, # lines, font
    while (BusyXLCD()); // Wait if LCD busy
    WriteCmdXLCD(lcdtype); // Function set cmd

    // Turn the display on then off
    while (BusyXLCD()); // Wait if LCD busy
    WriteCmdXLCD(LCD_DOFF & LCD_CURSOR_OFF & LCD_BLINK_OFF); // Display OFF/Blink OFF
    while (BusyXLCD()); // Wait if LCD busy
    WriteCmdXLCD(LCD_DON & LCD_CURSOR_ON & LCD_BLINK_ON); // Display ON/Blink ON

    // Clear display
    while (BusyXLCD()); // Wait if LCD busy
    WriteCmdXLCD(0x01); // Clear display

    // Set entry mode inc, no shift
    while (BusyXLCD()); // Wait if LCD busy
    WriteCmdXLCD(LCD_SHIFT_CUR_LEFT); // Entry Mode

    // Set DD Ram address to 0
    while (BusyXLCD()); // Wait if LCD busy
    SetDDRamAddr(0x80); // Set Display data ram address to 0

    return;
}

