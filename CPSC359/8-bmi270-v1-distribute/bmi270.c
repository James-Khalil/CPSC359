// bmi270.c

#include "bmi270.h"
#include "bmi270config.h"
#include "i2c.h"
#include "uart.h"
#include "timer.h"


//
// BMI270 is at 0x68 or 0x69
//
//  initialize BMI270 - save I2C address and check  ID register to verify that it is there
//
static unsigned char bmiAddress = 0x68;

// BMI270 config info
static int bmiAccOdr = 0;
static int bmiGyrOdr = 0;
static int bmiAccRange = 0;
static int bmiGyrRange = 0;

// initialize BMI270
int bmi270Init( unsigned addr ) {
    // save device address
    bmiAddress = addr;
    
    // check ID - return error flag
    unsigned char id = i2cReadByteData( bmiAddress, CHIP_ID_ADDRESS );
    if ( id == 0x24 ) {
        return 0;       // 0 = no error
    } else {
        return 1;       // 1 = error
    }
}


//
// load config file to initialize BMI270 after power up
//
void bmi270LoadConfigFile() {
    if ( (i2cReadByteData( bmiAddress, INTERNAL_STATUS ) & 0x0f) == 0x01 ) {
        uart_puts( "  --> Initialization already done\n" );
    } else {
        uart_puts( "  --> Initializating\n" );
        i2cWriteByteData( bmiAddress, PWR_CONF, 0x00 );
        delayus( 450 );
        i2cWriteByteData( bmiAddress, INIT_CTRL, 0x00 );
        
        i2cWriteByteData( bmiAddress, INIT_ADDR_0, 0x00 );
        i2cWriteByteData( bmiAddress, INIT_ADDR_1, 0x00 );
        i2cWriteBlockData( bmiAddress, INIT_DATA, bmi270ConfigFile, 8192 );
        delayus( 20 );
        i2cWriteByteData( bmiAddress, INIT_CTRL, 0x01 );
        delay( 20 );
        
        if ( (i2cReadByteData( bmiAddress, INTERNAL_STATUS ) & 0x0f) == 0x01 ) {
            uart_puts( "  --> Initialization successful\n" );
        } else {
            uart_puts( "  --> Initialization failed\n" );
        }
    }
    
}

//
// set BMI270 mode
//  mode:
//      LOW_POWER_MODE      'l'
//      NORMAL_MODE         'n'
//      PERFORMANCE_MODE    'p'
//
void bmi270SetMode( char mode ) {
    switch( mode ) {
        case LOW_POWER_MODE :   // low-power
            i2cWriteByteData( bmiAddress, PWR_CTRL, 0x04 );
            i2cWriteByteData( bmiAddress, ACC_CONF, 0x17 );
            i2cWriteByteData( bmiAddress, GYR_CONF, 0x28 );
            i2cWriteByteData( bmiAddress, PWR_CONF, 0x03 );
            bmiAccOdr = 50;
            bmiGyrOdr = 100;
            uart_puts( " --> Mode set to: LOW_POWER_MODE\n" );
            break;
        case NORMAL_MODE :   // normal
            i2cWriteByteData( bmiAddress, PWR_CTRL, 0x0E );
            i2cWriteByteData( bmiAddress, ACC_CONF, 0xA8 );
            i2cWriteByteData( bmiAddress, GYR_CONF, 0xA9 );
            i2cWriteByteData( bmiAddress, PWR_CONF, 0x02 );
            bmiAccOdr = 100;
            bmiGyrOdr = 200;
            uart_puts( " --> Mode set to: NORMAL_MODE\n" );
            break;
        case PERFORMANCE_MODE :   // performance
            i2cWriteByteData( bmiAddress, PWR_CTRL, 0x0E );
            i2cWriteByteData( bmiAddress, ACC_CONF, 0xA8 );
            i2cWriteByteData( bmiAddress, GYR_CONF, 0xE9 );
            i2cWriteByteData( bmiAddress, PWR_CONF, 0x02 );
            bmiAccOdr = 100;
            bmiGyrOdr = 200;
            uart_puts( " --> Mode set to: PERFORMANCE_MODE\n" );
            break;
         default :
            uart_puts( "Wrong mode. Use 'l' (low_power), 'n' (normal) or 'p'(performance)\n" );
    }
}



//
// set accelerometer range
//
// range must be in:
//      ACC_RANGE_2G    +/- 2g
//      ACC_RANGE_4G    +/- 4g
//      ACC_RANGE_8G    +/- 8g
//      ACC_RANGE_16G   +/- 16g
//
void bmi270SetAccRange( int range ) {
    switch ( range ) {
        case ACC_RANGE_2G :
            i2cWriteByteData( bmiAddress, ACC_RANGE, ACC_RANGE_2G );
            bmiAccRange = 2;
            uart_puts( " --> ACC range set to: 2G\n" );
            break;
        case ACC_RANGE_4G :
            i2cWriteByteData( bmiAddress, ACC_RANGE, ACC_RANGE_4G );
            bmiAccRange = 4;
            uart_puts( " --> ACC range set to: 4G\n" );
            break;
        case ACC_RANGE_8G :
            i2cWriteByteData( bmiAddress, ACC_RANGE, ACC_RANGE_8G );
            bmiAccRange = 8;
            uart_puts( " --> ACC range set to: 8G\n" );
            break;
        case ACC_RANGE_16G :
            i2cWriteByteData( bmiAddress, ACC_RANGE, ACC_RANGE_16G );
            bmiAccRange = 16;
            uart_puts( " --> ACC range set to: 16G\n" );
            break;
        default :
            uart_puts( "Wrong ACC range. Use ACC_RANGE_{2G|4G|8G|16G}\n" );
    }
}

//
// set gyro range
//
// range must be in:
//      GYR_RANGE_2000  // +/- 2000dps,  16.4 LSB/dps
//      GYR_RANGE_1000  // +/- 1000dps,  32.8 LSB/dps
//      GYR_RANGE_500   // +/- 500dps,   65.6 LSB/dps
//      GYR_RANGE_250   // +/- 250dps,  131.2 LSB/dps
//      GYR_RANGE_125   // +/- 125dps,  262.4 LSB/dps
//
void bmi270SetGyrRange( int range ) {
    switch ( range ) {
        case GYR_RANGE_2000 :
            i2cWriteByteData( bmiAddress, GYR_RANGE, GYR_RANGE_2000 );
            bmiGyrRange = 2000;
            uart_puts( " --> GYR range set to: 2000\n" );
            break;
        case GYR_RANGE_1000 :
            i2cWriteByteData( bmiAddress, GYR_RANGE, GYR_RANGE_1000 );
            bmiGyrRange = 1000;
            uart_puts( " --> GYR range set to: 1000\n" );
            break;
        case GYR_RANGE_500 :
            i2cWriteByteData( bmiAddress, GYR_RANGE, GYR_RANGE_500 );
            bmiGyrRange = 500;
            uart_puts( " --> GYR range set to: 500\n" );
            break;
        case GYR_RANGE_250 :
            i2cWriteByteData( bmiAddress, GYR_RANGE, GYR_RANGE_250 );
            bmiGyrRange = 250;
            uart_puts( " --> GYR range set to: 250\n" );
            break;
        case GYR_RANGE_125 :
            i2cWriteByteData( bmiAddress, GYR_RANGE, GYR_RANGE_125 );
            bmiGyrRange = 125;
            uart_puts( " --> GYR range set to: 125\n" );
            break;
        default :
            uart_puts( "Wrong GYR range. Use GYR_RANGE_{2000|1000|500|250|125\n ");
    }
}


//
// set acclerometer output data rate (ODR)
//
// rate must be in:
//      ACC_ODR_1600    1600Hz
//      ACC_ODR_800      800Hz
//      ACC_ODR_400      400Hz
//      ACC_ODR_200      200Hz
//      ACC_ODR_100      100Hz
//      ACC_ODR_50        50Hz
//      ACC_ODR_25        25Hz
//
void bmi270SetAccOdr( int odr ) {
    unsigned char r;
    switch( odr ) {
        case ACC_ODR_1600 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_1600 );
            bmiAccOdr = 1600;
            uart_puts( " --> ACC ODR set to: 1600\n" );
            break;
        case ACC_ODR_800 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_800 );
            bmiAccOdr = 800;
            uart_puts( " --> ACC ODR set to: 800\n" );
            break;
        case ACC_ODR_400 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_400 );
            bmiAccOdr = 400;
            uart_puts( " --> ACC ODR set to: 400\n" );
            break;
        case ACC_ODR_200 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_200 );
            bmiAccOdr = 200;
            uart_puts( " --> ACC ODR set to: 200\n" );
            break;
        case ACC_ODR_100 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_100 );
            bmiAccOdr = 100;
            uart_puts( " --> ACC ODR set to: 100\n" );
            break;
        case ACC_ODR_50 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_50 );
            bmiAccOdr = 50;
            uart_puts( " --> ACC ODR set to: 50\n" );
            break;
        case ACC_ODR_25 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r  & 0xf0) | ACC_ODR_1600 );
            bmiAccOdr = 25;
            uart_puts( " --> ACC ODR set to: 25\n" );
            break;
        default :
            uart_puts( "Wrong ACC ODR. Use ACC_ODR_{1600|800|400|200|100|50|25}\n" );
    }
}


//
// set gyro output data rate (ODR)
//
// rate must be in:
//      GYR_ODR_3200    3200Hz
//      GYR_ODR_1600    1600Hz
//      GYR_ODR_800     800Hz
//      GYR_ODR_400     400Hz
//      GYR_ODR_200     200Hz
//      GYR_ODR_100     100Hz
//      GYR_ODR_50      50Hz
//      GYR_ODR_25      25Hz
//
void bmi270SetGyrOdr( int odr ) {
    unsigned int r;
    
    switch ( odr ) {
        case GYR_ODR_3200 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_3200 );
            bmiGyrOdr = 3200;
            uart_puts( " --> GYR ODR set to: 3200\n" );
            break;
        case GYR_ODR_1600 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_1600 );
            bmiGyrOdr = 1600;
            uart_puts( " --> GYR ODR set to: 1600\n" );
            break;
        case GYR_ODR_800 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_800 );
            bmiGyrOdr = 800;
            uart_puts( " --> GYR ODR set to: 800\n" );
            break;
        case GYR_ODR_400 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_400 );
            bmiGyrOdr = 400;
            uart_puts( " --> GYR ODR set to: 400\n" );
            break;
        case GYR_ODR_200 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_200 );
            bmiGyrOdr = 200;
            uart_puts( " --> GYR ODR set to: 200\n" );
            break;
        case GYR_ODR_100 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_100 );
            bmiGyrOdr = 100;
            uart_puts( " --> GYR ODR set to: 100\n" );
            break;
        case GYR_ODR_50 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_50 );
            bmiGyrOdr = 50;
            uart_puts( " --> GYR ODR set to: 50\n" );
            break;
        case GYR_ODR_25 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xf0) | GYR_ODR_25 );
            bmiGyrOdr = 25;
            uart_puts( " --> GYR ODR set to: 25\n" );
            break;
        default :
            uart_puts( "Wrong GYR ODR. Use GYR_ODR_{3200|1600|800|400|200|100|50|25}\n" );
    }
}


//
// set accelerometer bandwidth parameter (BWP)
//
// bwp must be in:
//      ACC_BWP_OSR4    OSR4
//      ACC_BWP_OSR2    OSR2
//      ACC_BWP_NORMAL  Normal
//      ACC_BWP_CIC     CIC
//      ACC_BWP_RES16   Reserved
//      ACC_BWP_RES32   Reserved
//      ACC_BWP_RES64   Reserved
//      ACC_BWP_RES128  Reserved
//
void bmi270SetAccBwp( int bwp ) {
    unsigned int r;
    
    switch( bwp ) {
        case ACC_BWP_OSR4 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_OSR4 << 4) );
            uart_puts( " --> ACC BWP set to: OSR4\n" );
            break;
        case ACC_BWP_OSR2 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_OSR2 << 4) );
            uart_puts( " --> ACC BWP set to: OSR2\n" );
            break;
        case ACC_BWP_NORMAL :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_NORMAL << 4) );
            uart_puts( " --> ACC BWP set to: NORMAL\n" );
            break;
        case ACC_BWP_CIC :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_CIC << 4) );
            uart_puts( " --> ACC BWP set to: CIC\n" );
            break;
        case ACC_BWP_RES16 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_RES16 << 4) );
            uart_puts( " --> ACC BWP set to: RES16\n" );
            break;
        case ACC_BWP_RES32 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_RES32 << 4) );
            uart_puts( " --> ACC BWP set to: RES32\n" );
            break;
        case ACC_BWP_RES64 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_RES64 << 4) );
            uart_puts( " --> ACC BWP set to: RES64\n" );
            break;
        case ACC_BWP_RES128 :
            r = i2cReadByteData( bmiAddress, ACC_CONF );
            i2cWriteByteData( bmiAddress, ACC_CONF, (r & 0x8f) | (ACC_BWP_RES128 << 4) );
            uart_puts( " --> ACC BWP set to: RES28\n" );
            break;
        default :
            uart_puts( "Wrong ACC BWP. Use ACC_BWP_{OSR4|OSR2|NORMAL|CIC|RES16|RES32|RES64|RES128}\n" );
    }
}

//
// set gyro bandwidth parameter (BWP)
//
// bwp must be in:
//      GYR_BWP_OSR4    OSR4
//      GYR_BWP_OSR2    OSR2
//      GYR_BWP_NORMAL  Normal
//
void bmi270SetGyrBwp( int bwp ) {
    unsigned int r;
    
    switch( bwp ) {
        case GYR_BWP_OSR4 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xcf ) | (GYR_BWP_OSR4 << 4) );
            uart_puts( " --> GYR BWP set to: OSR4\n" );
            break;
        case GYR_BWP_OSR2 :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xcf ) | (GYR_BWP_OSR2 << 4) );
            uart_puts( " --> GYR BWP set to: OSR2\n" );
            break;
        case GYR_BWP_NORMAL :
            r = i2cReadByteData( bmiAddress, GYR_CONF );
            i2cWriteByteData( bmiAddress, GYR_CONF, (r & 0xcf ) | (GYR_BWP_NORMAL << 4) );
            uart_puts( " --> GYR BWP set to: NORMAL\n" );
            break;
        default :
            uart_puts( "Wrong GYR BWP. Use 'GYR_BWP_{OSR4|OSR2|NORMAL}\n" );
    }
}


//
// disable the FIFO header mode
//
void bmi270DisableFifoHeader() {
    unsigned int r;
    
    r = i2cReadByteData( bmiAddress, FIFO_CONFIG_1 );
    i2cWriteByteData( bmiAddress, FIFO_CONFIG_1, r & 0b11101111 );      // clear bit 4
    uart_puts( " --> FIFO Header disabled (ODR of all enabled sensors need to be identical)\n" );
}

//
// enable data streaming
//
void bmi270EnableDataStreaming() {
    unsigned int r;
    
    r = i2cReadByteData( bmiAddress, FIFO_CONFIG_1 );
    i2cWriteByteData( bmiAddress, FIFO_CONFIG_1, r | 0b11100000 );
    uart_puts( " --> Streaming Mode enabled (no data will be stored in FIFO)\n" );
}

//
// enable accelerometer filter performance
//
void bmi270EnableAccFilterPerf() {
    unsigned int r;
    
    r = i2cReadByteData( bmiAddress, ACC_CONF );
    i2cWriteByteData( bmiAddress, ACC_CONF, r | 0b10000000 );
    uart_puts( " --> Accelerometer filter performance enabled (performance optimized)\n" );
}


//
// enable gyro noise performance
//
void bmi270EnableGyrNoisePerf( ) {
    unsigned int r;
    
    r = i2cReadByteData( bmiAddress, GYR_CONF );
    i2cWriteByteData( bmiAddress, GYR_CONF, r | 0b01000000 );
    uart_puts( " --> Gyroscope noise performance enabled (performance optimized)\n" );
}



//
// enable gyro filter performance
//
void bmi270EnableGyrFilterPerf( ) {
    unsigned int r;
    
    r = i2cReadByteData( bmiAddress, GYR_CONF );
    i2cWriteByteData( bmiAddress, GYR_CONF, r | 0b10000000 );
    uart_puts( " --> Gyroscope filter performance enabled (performance optimized)\n" );
}


//
// get raw accelerometer data
//
void bmi270GetRawAccData( short *raw ) {
/*
    unsigned short xLo = i2cReadByteData( bmiAddress, ACC_X_LO );
    unsigned short xHi = i2cReadByteData( bmiAddress, ACC_X_HI );
    unsigned short yLo = i2cReadByteData( bmiAddress, ACC_Y_LO );
    unsigned short yHi = i2cReadByteData( bmiAddress, ACC_Y_HI );
    unsigned short zLo = i2cReadByteData( bmiAddress, ACC_Z_LO );
    unsigned short zHi = i2cReadByteData( bmiAddress, ACC_Z_HI );
    vals[0] = xHi << 8 | xLo;
    vals[1] = yHi << 8 | yLo;
    vals[2] = zHi << 8 | zLo;
*/
    unsigned char bytes[6];
    i2cReadBlockData( bmiAddress, ACC_X_LO, 6, bytes );
    raw[0] = bytes[1] << 8 | bytes[0];
    raw[1] = bytes[3] << 8 | bytes[2];
    raw[2] = bytes[5] << 8 | bytes[4];
}

//
// get accelerometer data in milli-G
//
void bmi270GetAccData( int *vals ) {
    short raw[3];
    
    bmi270GetRawAccData( raw );
    
    vals[0] = ( raw[0] * 1000 * bmiAccRange ) >> 15;
    vals[1] = ( raw[1] * 1000 * bmiAccRange ) >> 15;
    vals[2] = ( raw[2] * 1000 * bmiAccRange ) >> 15;
}


//
// get raw gyro data
//
void bmi270GetRawGyrData( short *raw ) {
    unsigned char bytes[6];
    i2cReadBlockData( bmiAddress, GYR_X_LO, 6, bytes );
    raw[0] = bytes[1] << 8 | bytes[0];
    raw[1] = bytes[3] << 8 | bytes[2];
    raw[2] = bytes[5] << 8 | bytes[4];
}

//
// get gyro data in degrees-per-second
//
void bmi270GetGyrData( int *vals ) {
    short raw[3];
    
    bmi270GetRawGyrData( raw );
    
    vals[0] = ( raw[0] *  bmiGyrRange ) >> 15;
    vals[1] = ( raw[1] *  bmiGyrRange ) >> 15;
    vals[2] = ( raw[2] *  bmiGyrRange ) >> 15;
}


//
// get raw accel and gyro data
//
void bmi270GetRawAllData( short *raw ) {
    unsigned char bytes[12];
    i2cReadBlockData( bmiAddress, ACC_X_LO, 12, bytes );
    raw[0] = bytes[ 1] << 8 | bytes[ 0];
    raw[1] = bytes[ 3] << 8 | bytes[ 2];
    raw[2] = bytes[ 5] << 8 | bytes[ 4];
    raw[3] = bytes[ 7] << 8 | bytes[ 6];
    raw[4] = bytes[ 9] << 8 | bytes[ 8];
    raw[5] = bytes[11] << 8 | bytes[10];
}

//
// get accel and gyro data in milliG and degrees-per-second
//
void bmi270GetAllData( int *vals ) {
    short raw[6];
    
    bmi270GetRawAllData( raw );
    
    vals[0] = ( raw[0] * 1000 * bmiAccRange ) >> 15;
    vals[1] = ( raw[1] * 1000 * bmiAccRange ) >> 15;
    vals[2] = ( raw[2] * 1000 * bmiAccRange ) >> 15;
    vals[3] = ( raw[3] *  bmiGyrRange ) >> 15;
    vals[4] = ( raw[4] *  bmiGyrRange ) >> 15;
    vals[5] = ( raw[5] *  bmiGyrRange ) >> 15;
}



