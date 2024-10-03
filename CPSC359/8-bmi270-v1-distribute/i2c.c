// i2c.c

#include "i2c.h"
#include "timer.h"

//
// I2C = BSC (Broadcomm Serial Controller)
//

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

// I2C pins
#define BSC1_SDA 2
#define BSC1_SCL 3


// init pins for I2C1
void initI2CPins() {
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



unsigned char i2cReadByteData( unsigned char devAddr, unsigned char regAddr ) {
    volatile unsigned int *bsc1 = (unsigned int *) BSC1_BASE;
    
    // start transfer to I2C/BSC device to reguest register address
    bsc1[ BSC_A ] = devAddr;        // A = device address
    bsc1[ BSC_DLEN ] = 1;    		// transfer length is 1 byte
    bsc1[ BSC_FIFO ] = regAddr;     // put the in-device register address into FIFO
    bsc1[ BSC_S ] =    (1 << 9) | (1 << 8) | (1 << 1);      // clear CLKT, ERR, DONE in status register
    bsc1[ BSC_C ] =    (1 << 15) | (1 << 7);    // enable controller and start transfer in control register, read bit 0

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
    delayus( 50 );

    return result;
}



void i2cReadBlockData( unsigned char devAddr, unsigned char regAddr,
                int n, unsigned char *result ) {
    volatile unsigned int *bsc1 = (unsigned int *) BSC1_BASE;

    // code removed
}



void i2cWriteByteData( unsigned char devAddr, unsigned char regAddr, unsigned char val) {
    volatile unsigned int *bsc1 = (unsigned int *) BSC1_BASE;

    // start transfer to I2C/BSC device to reguest register address
    //bsc1[ BSC_C ] = 1 << 4;
    bsc1[ BSC_A ] = devAddr;            // A = device address
    bsc1[ BSC_DLEN ] = 2;               // two bytes - register plus data
    bsc1[ BSC_FIFO ] = regAddr;         // value of first register
    bsc1[ BSC_FIFO ] = val;             // value of first register
    bsc1[ BSC_S ] =    (1 << 9) | (1 << 8) | (1 << 1);  // clear CLKT, ERR, DONE in status register
    bsc1[ BSC_C ] =    (1 << 15) | (1 << 7);     // enable controller and start transfer in control register, read bit 0

    // wait for send to finish
    // wait loop until done - poll DONE bit in status register
    while( !(bsc1[ BSC_S ] & 0x02) );
    
    delayus( 100 );
}


void i2cWriteBlockData( char devAddr, unsigned char regAddr, unsigned char *val, int count ) {
    volatile unsigned int *bsc1 = (unsigned int *) BSC1_BASE;

    // start transfer to I2C/BSC device to reguest register address
    bsc1[ BSC_A ] = devAddr;            // A = device address
    bsc1[ BSC_DLEN ] = count + 1;       // register plus data
    bsc1[ BSC_FIFO ] = regAddr;         // value of first register
    bsc1[ BSC_S ] =    (1 << 9) | (1 << 8) | (1 << 1);      // clear CLKT, ERR, DONE in status register
    bsc1[ BSC_C ] =    (1 << 15) | (1 << 7);     // enable controller and start transfer in control register, read bit 0

    // write values into FIFO as space is available
    for( int i = 0; i < count; i++ ) {
        while ( (bsc1[ BSC_S ] & (1 << 4)) == 0 );  // wait while FIFO full
        bsc1[ BSC_FIFO ] = val[i];                  // put value in FIFO to go out
    }
    
    // wait for send to finish
    // wait loop until done - poll DONE bit in status register
    while( !(bsc1[ BSC_S ] & 0x02) );
    delayus( 50 );
}


