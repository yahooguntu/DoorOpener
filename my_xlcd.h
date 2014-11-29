#ifndef __XLCD_H
#define __XLCD_H
#include "p18cxxx.h"
#include "i2c_rw.h"
/* PIC18 XLCD peripheral routines.
 *
 *   Notes:
 *      - These libraries routines are written to support the
 *        Hitachi HD44780 LCD controller.
 *      - The user must define the following items:
 *          - The LCD interface type (4- or 8-bits)
 *          - If 4-bit mode
 *              - whether using the upper or lower nibble
 *          - The data port
 *              - The tris register for data port
 *              - The control signal ports and pins
 *              - The control signal port tris and pins
 *          - The user must provide three delay routines:
 *              - DelayFor18TCY() provides a 18 Tcy delay
 *              - DelayPORXLCD() provides at least 15ms delay
 *              - DelayXLCD() provides at least 5ms delay
 */

/* I2C addresses of the port expander. */
#define EXPANDER_ADDR_R     0x41
#define EXPANDER_ADDR_W     0x40
#define EXPANDER_GPIOA      0x12
#define EXPANDER_GPIOB      0x13
#define EXPANDER_IODIRA     0x00
#define EXPANDER_IODIRB     0x01
#define EXPANDER_OLATA      0x14
#define EXPANDER_OLATB      0x15

/* Pins:
 *  GPIOA:
 *      0: Select
 *      1: Right
 *      2: Down
 *      3: Up
 *      4: Left
 *      5: 
 *      6: Red
 *      7: Green
 *  GPIOB:
 *      0: Blue
 *      1: DB7
 *      2: DB6
 *      3: DB5
 *      4: DB4
 *      5: E
 *      6: RW
 *      7: RS
 */

/* LCD control values. */

/* Display ON/OFF Control defines */
#define LCD_DON         0b11110000  /* Display on      */
#define LCD_DOFF        0b11010000  /* Display off     */
#define LCD_CURSOR_ON   0b11110000  /* Cursor on       */
#define LCD_CURSOR_OFF  0b10110000  /* Cursor off      */
#define LCD_BLINK_ON    0b11110000  /* Cursor Blink    */
#define LCD_BLINK_OFF   0b01110000  /* Cursor No Blink */

/* Cursor or Display Shift defines */
#define LCD_SHIFT_CUR_LEFT    0b00100000  /* Cursor shifts to the left   */
#define LCD_SHIFT_CUR_RIGHT   0b10100000  /* Cursor shifts to the right  */
#define LCD_SHIFT_DISP_LEFT   0b01100000  /* Display shifts to the left  */
#define LCD_SHIFT_DISP_RIGHT  0b11100000  /* Display shifts to the right */

/* Function Set defines */
#define LCD_FOUR_BIT   0b00110100  /* 4-bit Interface               */
#define LCD_EIGHT_BIT  0b00111100  /* 8-bit Interface               */
#define LCD_LINE_5X7   0b00001100  /* 5x7 characters, single line   */
#define LCD_LINE_5X10  0b00101100  /* 5x10 characters               */
#define LCD_LINES_5X7  0b00011100  /* 5x7 characters, multiple line */

#define PARAM_SCLASS auto
#define MEM_MODEL far  /* Change this to near for small memory model */


/* OpenXLCD
 * Configures I/O pins for external LCD
 */
void OpenXLCD(PARAM_SCLASS unsigned char);

/* SetCGRamAddr
 * Sets the character generator address
 */
void SetCGRamAddr(PARAM_SCLASS unsigned char);

/* SetDDRamAddr
 * Sets the display data address
 */
void SetDDRamAddr(PARAM_SCLASS unsigned char);

/* BusyXLCD
 * Returns the busy status of the LCD
 */
unsigned char BusyXLCD(void);

/* ReadAddrXLCD
 * Reads the current address
 */
unsigned char ReadAddrXLCD(void);

/* ReadDataXLCD
 * Reads a byte of data
 */
char ReadDataXLCD(void);

/* WriteCmdXLCD
 * Writes a command to the LCD
 */
void WriteCmdXLCD(PARAM_SCLASS unsigned char);

/* WriteDataXLCD
 * Writes a data byte to the LCD
 */
void WriteDataXLCD(PARAM_SCLASS char);

/* putcXLCD
 * A putc is a write
 */
#define putcXLCD WriteDataXLCD

/* putsXLCD
 * Writes a string of characters to the LCD
 */
void putsXLCD(PARAM_SCLASS char *);

/* putrsXLCD
 * Writes a string of characters in ROM to the LCD
 */
void putrsXLCD(const rom char *);

/* User defines these routines according to the oscillator frequency */
extern void DelayFor18TCY(void);
extern void DelayPORXLCD(void);
extern void DelayXLCD(void);

#endif
