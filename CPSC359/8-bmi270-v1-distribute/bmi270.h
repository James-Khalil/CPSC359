// bmi270.h

#ifndef BMI270
#define BMI270



// General
#define CHIP_ID_ADDRESS   0x00   
#define SENSORTIME_0      0x18   
#define SENSORTIME_1      0x19   
#define SENSORTIME_2      0x1A   
#define INTERNAL_STATUS   0x21   
#define DATA_REG          0x0C   
#define FIFO_CONFIG_0     0x48   
#define FIFO_CONFIG_1     0x49   
#define INIT_CTRL         0x59   
#define INIT_ADDR_0       0x5B   
#define INIT_ADDR_1       0x5C   
#define INIT_DATA         0x5E   
#define CMD               0x7E   
#define PWR_CONF          0x7C   
#define PWR_CTRL          0x7D   

// Accelerometer
#define ACC_CONF          0x40   
#define ACC_RANGE         0x41   
#define ACC_X_LO          0x0C   
#define ACC_X_HI          0x0D   
#define ACC_Y_LO          0x0E   
#define ACC_Y_HI          0x0F   
#define ACC_Z_LO          0x10   
#define ACC_Z_HI          0x11   

// Gyroscope
#define GYR_CONF          0x42   
#define GYR_RANGE         0x43   
#define GYR_X_LO          0x12   
#define GYR_X_HI          0x13   
#define GYR_Y_LO          0x14   
#define GYR_Y_HI          0x15   
#define GYR_Z_LO          0x16   
#define GYR_Z_HI          0x17   

// Temperature
#define TEMP_LO           0x22   
#define TEMP_HI           0x23   



// -------------------------------------------------
// DEFINITIONS
// -------------------------------------------------

// General
#define GRAVITY         9.81288

// Device Modes
#define LOW_POWER_MODE      'l'
#define NORMAL_MODE         'n'
#define PERFORMANCE_MODE    'p'

// Accelerometer
#define ACC_RANGE_2G    0x00      // +/- 2g
#define ACC_RANGE_4G    0x01      // +/- 4g
#define ACC_RANGE_8G    0x02      // +/- 8g
#define ACC_RANGE_16G   0x03      // +/- 16g
#define ACC_ODR_1600    0x0C      // 1600Hz
#define ACC_ODR_800     0x0B      // 800Hz
#define ACC_ODR_400     0x0A      // 400Hz
#define ACC_ODR_200     0x09      // 200Hz
#define ACC_ODR_100     0x08      // 100Hz
#define ACC_ODR_50      0x07      // 50Hz
#define ACC_ODR_25      0x06      // 25Hz
#define ACC_BWP_OSR4    0x00      // OSR4
#define ACC_BWP_OSR2    0x01      // OSR2
#define ACC_BWP_NORMAL  0x02      // Normal
#define ACC_BWP_CIC     0x03      // CIC
#define ACC_BWP_RES16   0x04      // Reserved
#define ACC_BWP_RES32   0x05      // Reserved
#define ACC_BWP_RES64   0x06      // Reserved
#define ACC_BWP_RES128  0x07      // Reserved


// Gyroscope
#define GYR_RANGE_2000  0x00      // +/- 2000dps,  16.4 LSB/dps
#define GYR_RANGE_1000  0x01      // +/- 1000dps,  32.8 LSB/dps
#define GYR_RANGE_500   0x02      // +/- 500dps,   65.6 LSB/dps
#define GYR_RANGE_250   0x03      // +/- 250dps,  131.2 LSB/dps
#define GYR_RANGE_125   0x04      // +/- 125dps,  262.4 LSB/dps
#define GYR_ODR_3200    0x0D      // 3200Hz
#define GYR_ODR_1600    0x0C      // 1600Hz
#define GYR_ODR_800     0x0B      // 800Hz
#define GYR_ODR_400     0x0A      // 400Hz
#define GYR_ODR_200     0x09      // 200Hz
#define GYR_ODR_100     0x08      // 100Hz
#define GYR_ODR_50      0x07      // 50Hz
#define GYR_ODR_25      0x06      // 25Hz
#define GYR_BWP_OSR4    0x00      // OSR4
#define GYR_BWP_OSR2    0x01      // OSR2
#define GYR_BWP_NORMAL  0x02      // Normal



int bmi270Init( unsigned addr );
void bmi270LoadConfigFile();
void bmi270SetMode( char mode );
void bmi270SetAccRange( int range );
void bmi270SetGyrRange( int range );
void bmi270SetAccOdr( int odr );
void bmi270SetGyrOdr( int odr );
void bmi270SetAccBwp( int bwp );
void bmi270SetGyrBwp( int bwp );
void bmi270DisableFifoHeader();
void bmi270EnableDataStreaming();
void bmi270EnableAccFilterPerf();
void bmi270EnableGyrNoisePerf( );
void bmi270EnableGyrFilterPerf( );

void bmi270GetRawAccData( short *vals );
void bmi270GetAccData( int *vals );
void bmi270GetRawGyrData( short *vals );
void bmi270GetGyrData( int *vals );
void bmi270GetRawAllData( short *vals );
void bmi270GetAllData( int *vals );

#endif
    

