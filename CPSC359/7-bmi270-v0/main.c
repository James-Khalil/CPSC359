// 2-gpio-test  main.c - demo the gpio pins


#include <unistd.h>
#include <stdio.h>
//#include "gpio.h"
#include "uart.h"

// gpio register defs
#define GPIO_BASE 0xFE200000
#define GPIO_PUP_PDN_CNTRL_REG0 57

// BSC1  (I2C-1) registers and pins
#define BSC1_BASE 0xFE804000
#define BSC_C    0
#define BSC_S    1
#define BSC_DLEN 2
#define BSC_A    3
#define BSC_FIFO 4
#define BSC_DIV  5
#define BSC_DEL  6
#define BSC_CLKT 7

#define BSC1_SDA 2
#define BSC1_SCL 3




// convert unsigned int into character string in base 16
void xtoa( unsigned int n, char *s, int size ) {
    int i;

    // start with null termination - we are interting things backwards
    s[0] = 0;

    // insert the digits starting with least significant
    i = 1;      // start after the null termination
    do {
        int lsdigit = n % 16;
        if ( lsdigit < 10 )
            s[i++] = lsdigit % 10 + '0';
        else
            s[i++] = lsdigit-10 + 'a';
        n /= 16;
    } while( n != 0  && i < size );

    // reverse string
    char temp;
    i--;
    for( int j = 0; j < i; j++ ) {
        temp = s[j];
        s[j] = s[i];
        s[i--] = temp;
    }
}


void initBSC1Pins() {
    volatile unsigned int *gpio = (unsigned int *) GPIO_BASE;
    
    // activate pull-ups for BSC1 pins
    gpio[GPIO_PUP_PDN_CNTRL_REG0 + BSC1_SDA/16] &= ~(0b11 << (2*(BSC1_SDA % 16)));     // clear pull-up-down (no resistor)
    gpio[GPIO_PUP_PDN_CNTRL_REG0 + BSC1_SCL/16] &= ~(0b11 << (2*(BSC1_SCL % 16)));     // clear pull-up-down (no resistor)


    // init BSC1 pin functions
    gpio[BSC1_SDA/10] &= ~(0b111 << (3*(BSC1_SDA % 10)));         // clear function select bits (input)
    gpio[BSC1_SDA/10] |=   0b100 << (3*(BSC1_SDA % 10)) ;         // function select bits to ALT0
    
    gpio[BSC1_SCL/10] &= ~(0b111 << (3*(BSC1_SCL % 10)));         // clear function select bits (input)
    gpio[BSC1_SCL/10] |=   0b100 << (3*(BSC1_SCL % 10)) ;         // function select bits to ALT0
}



unsigned char i2cReadByteData( unsigned char dev, unsigned char reg ) {
    volatile unsigned int *bsc1 = (unsigned int *) BSC1_BASE;
    
    // start transfer to I2C/BSC device to reguest register address
    bsc1[ BSC_A ] = dev;            // A = device address
    bsc1[ BSC_DLEN ] = 1;    		// transfer length is 1 byte
    bsc1[ BSC_FIFO ] = reg;         // put the in-device register address into FIFO
    bsc1[ BSC_S ] =    (1 << 9) | (1 << 8) | (1 << 1); 	// clear CLKT, ERR, DONE in status register
    bsc1[ BSC_C ] =    (1 << 15) | (1 << 7);    	// enable controller and start transfer in control register, read bit 0

    // wait for send to finish
    // wait loop until done - poll DONE bit in status register
    while( !(bsc1[ BSC_S ] & 0x02) );

    // receive byte back from device
    bsc1[ BSC_DLEN ] = 1;       // receive one byte back
    bsc1[ BSC_S ] =    (1 << 9) | (1 << 8) | (1 << 1); 	// clear CLKT, ERR, DONE in status register
    bsc1[ BSC_C ] =    (1 << 15) | (1 << 7) | (1 << 4) | 1; // enable controller, start tx, clear FIFO, read bit set


    // wait for receive to finish
    // wait loop until done - poll DONE bit in status register
    while( !(bsc1[ BSC_S ] & 0x02) );

    // return result from FIFO register
    unsigned char result = bsc1[ BSC_FIFO ];
    return result;
}





int main() {
    unsigned char id;
    char printBuffer[256];
    

    uart_puts( "\n----------------------------------------\n\nWelcome\n\n" );
    
    uart_init();
    initBSC1Pins();
    
    uart_puts( "init done\n" );
    
    id = i2cReadByteData( 0x68, 0x00 );

    xtoa( id, printBuffer, 256 );
    uart_puts( "device id: 0x" );
    uart_puts( printBuffer );
    uart_puts( "\n\n" );
    
/*
    unsigned long long int tNext = 0;
    
    uart_init();
    initAudioPins();
    
    uart_puts( "\n\nLet's make some noise ...\n" );
    
    while( 1 ) {
        for( int i = 0; i < AMEN_LENGTH; i++ ) {
            pinOn( AUDIO_L_PIN );
            pinOn( AUDIO_R_PIN );

            register int nHi = (amen[i] * 416) >> 8;
            register int nLo = 103 - nHi;
            
            tNext = getSystemTimerCounter() + nHi;
            while ( getSystemTimerCounter() < tNext ) ;
            
            pinOff( AUDIO_L_PIN );
            pinOff( AUDIO_R_PIN );
            
            tNext = getSystemTimerCounter() + nLo;
            while ( getSystemTimerCounter() < tNext ) ;
        }
        uart_puts( "." );
    }
    */
    
    while(1);
}



