/*
 * File:   main.c
 * Author: Ethan Law and Mitch Birti
 *
 * Created on Nov 16, 2014, 11:35 AM
 */

#include <p18cxxx.h>
#include <delays.h>
#include <stdlib.h>
#include "my_xlcd.h"

#pragma config FOSC = INTIO67, PLLCFG = OFF, PRICLKEN = ON,FCMEN = ON, PWRTEN = OFF
#pragma config BOREN = SBORDIS, BORV = 250, WDTEN = OFF, WDTPS = 4096, PBADEN = OFF
#pragma config HFOFST = OFF, MCLRE = EXTMCLR, STVREN = ON, LVP = OFF, DEBUG = ON

unsigned char serialReadCount = 0;
unsigned char activeSerialBuffer = 0;
unsigned char isLocked; // set on initial lock
unsigned char idReady = 0;
unsigned char serialBuffer[2][10];


// unlocked duty cycle is 125 = 0x7D
unsigned int rom UNLOCKED_DC = 0x7D;
// locked duty cycle is 63
unsigned int rom LOCKED_DC = 0x3F;

unsigned char rom tags[][10] = {
    {'0', '7', '0', '0', '3', 'E', 'A', '8', '8', 'A'},
    {'5', '0', '0', '0', '3', 'D', 'F', 'F', '0', '4'}
};

// so the ISR doesn't get mad
unsigned char areBuffersIdentical(void);

//----------------------------------------------------------------
void low_ISR(void);
void high_ISR(void);
#pragma code low_vector = 0x08	// force the following statement to start at

void low_interrupt(void) // 0x08
{
    _asm
    goto low_ISR
            _endasm
}
#pragma code 			//return to the default code section
#pragma interrupt low_ISR

void low_ISR(void) {
    //HANDLE INTERRUPT

    if (idReady != 1) {
        unsigned char tmp = RC2REG; // read from RC2REG
        if (tmp == '\n' || tmp == '\r') {
            // ignore spurious newlines
            if (serialReadCount == 0) {
                return;
            }

            // switch buffers
            serialReadCount = 0;
            // flip activeSerialBuffer from 1 to zero or zero to 1
            activeSerialBuffer = 1 - activeSerialBuffer;

            // make sure we read the same tag twice
            if (areBuffersIdentical()) {
                idReady = 1;
            }
        } else {
            serialBuffer[activeSerialBuffer][serialReadCount] = tmp;
            serialReadCount++;
        }
    } else {
        // clear out the buffer
        unsigned char tmp = RC2REG;
    }




}
//-----------------------------------------------------------------

// --- Delays ---

void DelayFor18TCY(void) {
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
}

void DelayPORXLCD(void) {
    Delay1KTCYx(60); // Delay of 15ms
    // Cycles = (TimeDelay * Fosc) / 4
    // Cycles = (15ms * 16MHz) / 4
    // Cycles = 60,000
    return;
}

void DelayXLCD(void) {
    Delay1KTCYx(20); // Delay of 5ms
    // Cycles = (TimeDelay * Fosc) / 4
    // Cycles = (5ms * 16MHz) / 4
    // Cycles = 20,000
    return;
}

void Delay_1_sec(void) {
    Delay10KTCYx(25); // 1 sec
    return;
}

// --- Lock / unlock functions ---

void unlock(void) {
    enableBacklight(1, 1, 0);
    TRISD = TRISD & 0xFD; // turn on PWM
    CCPR4L = UNLOCKED_DC >> 2;
    CCP4CONbits.DC4B = UNLOCKED_DC & 0x0003;
    Delay_1_sec();
    TRISD = TRISD | 0x02; // turn PWM back off
    enableBacklight(0, 1, 0);
    isLocked = 0;
    SetDDRamAddr(0x00);
    putrsXLCD("Unlocked");
}

void lock(void) {
    enableBacklight(1, 1, 0);
    TRISD = TRISD & 0xFD; // turn on PWM
    CCPR4L = LOCKED_DC >> 2;
    CCP4CONbits.DC4B = LOCKED_DC & 0x0003;
    Delay_1_sec();
    TRISD = TRISD | 0x02; // turn PWM back off
    enableBacklight(1, 0, 0);
    isLocked = 1;
    SetDDRamAddr(0x00);
    putrsXLCD("Locked");
}

// used to make sure we read the same tag twice before setting idReady

unsigned char areBuffersIdentical(void) {
    unsigned char i;
    for (i = 0; i < 10; i++) {
        if (serialBuffer[0][i] != serialBuffer[1][i]) {
            return 0;
        }
    }
    return 1;
}

/**
 * NOTE - Uses current values of buffer and count
 * @return 1 if valid, 0 if invalid
 */
unsigned char isValid(void) {
    unsigned char i;
    unsigned char ti;
    unsigned char match;
    for (ti = 0; ti < 2; ti++) {
        match = 1;

        // see if any characters don't match
        for (i = 0; i < 10; i++) {
            if (serialBuffer[1 - activeSerialBuffer][i] != tags[ti][i]) {
                match = 0;
                break;
            }
        }

        if (match) {
            serialBuffer[1 - activeSerialBuffer][0] = '\n';
            return 1;
        }
    }

    // no matches found
    return 0;
}


// ----- MAIN -----

void main(void) {
    //make PIC18 run at 1MHz , Tcy=1MHz/4=250KHz.
    OSCCON2 = 0x00;
    OSCCON = 0x30;

    //Set PORTC as digital input for I2C
    TRISC = 0xFF;
    ANSELC = 0x00;

    // I2C config registers
    SSP1CON1 = 0x28;
    SSP1ADD = 0x0D; // baud rate, recalculate? started at 0x1F (7khz), now at ~18khz

    // configure for output (servo / buzzer)
    TRISD = 0x00;
    ANSELD = 0x00;
    PORTDbits.RD6 = 0; // turn off the buzzer!

    // set up RD7 (RX2/DT2) for digital input from serial card reader
    TRISDbits.RD7 = 1; // note - RD7 is pin 30 on PIC
    ANSELDbits.ANSD7 = 0;

    TRISDbits.RD1 = 1; // disable PWM

    RCSTA2 = 0x90; // enable serial receiver (from card reader)
    TXSTA2bits.SYNC = 0;
    PIE3bits.RC2IE = 1; // enable EUSART2 to get a receive interrupt
    SPBRG2 = 25; // Baud Rate = 2400
    TXSTA2bits.BRGH = 1; // baud rate config
    BAUDCON2 = 0x00; // set up baud rate, 8bit

    // use low priority interrupts
    RCONbits.IPEN = 0;
    INTCONbits.GIEL = 1;
    INTCONbits.GIEH = 1;

    // configure CCP for servo
    CCPTMRS1 = 0x00; // TIMER 2
    PR2 = 0xFF; //PWM Freq ~ 1kHz
    T2CON = 0x06; //1 post-scalar 16pre-scalar
    CCP4CON = 0x0C; // PWM mode


    // display setup
    OpenXLCD(LCD_FOUR_BIT & LCD_LINES_5X7);
    WriteCmdXLCD(LCD_SHIFT_DISP_LEFT);
    SetDDRamAddr(0x00);
    putrsXLCD("Test");

    lock();

    while (1) {
        if (idReady == 1) {

            if (isValid()) {
                // beep shortly
                PORTDbits.RD6 = 1;
                Delay1KTCYx(25);
                PORTDbits.RD6 = 0;

                I2C_Write(EXPANDER_OLATB, 0x00);

                // flip the lock state of the door
                if (isLocked) {
                    unlock();
                    isLocked = 0;
                } else {
                    lock();
                    isLocked = 1;
                }
            } else {
                // throw a fit with the beeper
                PORTDbits.RD6 = 1;
                Delay_1_sec();
                PORTDbits.RD6 = 0;
            }

            //            SetDDRamAddr(0x00);
            //            putrsXLCD("Unlocked");
            //            SetDDRamAddr(0x40);
            //            for(i = 0; i < count; i++) {
            //                putcXLCD(buffer[i]);
            //            }

            // debug - output to portD
            //PORTA = buffer[9];

            idReady = 0;
        }
        // TODO - lock the door back again

    }

}

