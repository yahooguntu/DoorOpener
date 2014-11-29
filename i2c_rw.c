/* 
 * File:   i2c_rw.c
 * Author: Yahooguntu
 *
 * Created on November 22, 2014, 2:27 PM
 */
#include <p18cxxx.h>
#include "my_xlcd.h"

unsigned char OLATA_curr;
unsigned char OLATB_curr;

void clockLCDHigh(void) {
    OLATB_curr = OLATB_curr | 0x20;
    I2C_Write(EXPANDER_OLATB, OLATB_curr);
}

void clockLCDLow(void) {
    OLATB_curr = OLATB_curr & 0xDF;
    I2C_Write(EXPANDER_OLATB, OLATB_curr);
}

void I2C_Write(unsigned char reg, unsigned char data) {
    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN); // cleared by hardware when start bit is done

    //unsigned char Slave_Address = 0x40;     // send out group address
    SSP1BUF = EXPANDER_ADDR_W; // last bit is read/write (0 = write)
    while (SSP1STATbits.R_NOT_W);

    if (SSP1CON2bits.ACKSTAT) {
        //TODO error handling
    }

    SSP1BUF = reg;
    while (SSP1STATbits.R_NOT_W);

    SSP1BUF = data;
    while (SSP1STATbits.R_NOT_W);

    SSP1CON2bits.PEN = 1;
    while (SSP1CON2bits.PEN); // stop
}

unsigned char I2C_Read(unsigned char reg) {
    SSP1CON2bits.SEN = 1; // start bit
    while (SSP1CON2bits.SEN);

    SSP1BUF = EXPANDER_ADDR_W; // I want to WRITE to a certain address
    while (SSP1STATbits.R_NOT_W); //edit - changed from SSP1CON2 to SSP1STAT

    SSP1BUF = reg;
    while (SSP1STATbits.R_NOT_W);

    SSP1CON2bits.RSEN = 1; // restart
    while (SSP1CON2bits.RSEN);

    SSP1BUF = EXPANDER_ADDR_R; // NOW I will read
    while (SSP1STATbits.R_NOT_W); //edit - changed from SSP1CON2 to SSP1STAT

    SSP1CON2bits.RCEN = 1; // init clock for reading
    while (SSP1CON2bits.RCEN); // wait for all 8 bits to be shifted in

    SSP1CON2bits.PEN = 1; // stop
    while (SSP1CON2bits.PEN);

    return SSP1BUF; // answer is in the buffer reg
}


void enableBacklight(unsigned char red, unsigned char green, unsigned char blue) {
    OLATB_curr = (OLATB_curr & 0xFE) | !blue;
    OLATA_curr = (OLATA_curr & 0x1F) | ((!red << 6) | (!green << 7));

    I2C_Write(EXPANDER_OLATA, OLATA_curr);
    I2C_Write(EXPANDER_OLATB, OLATB_curr);
}

unsigned char reverseNibbles(unsigned char b) {
   //b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}