/* 
 * File:   i2c_rw.h
 * Author: Yahooguntu
 *
 * Created on November 22, 2014, 10:02 PM
 */

#ifndef I2C_RW_H
#define	I2C_RW_H

#ifdef	__cplusplus
extern "C" {
#endif

extern unsigned char OLATA_curr;
extern unsigned char OLATB_curr;

unsigned char I2C_Read(unsigned char reg);
void I2C_Write(unsigned char reg, unsigned char data);

void enableBacklight(unsigned char red, unsigned char green, unsigned char blue);
unsigned char reverseNibbles(unsigned char b);

void clockLCDHigh(void);
void clockLCDLow(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_RW_H */

