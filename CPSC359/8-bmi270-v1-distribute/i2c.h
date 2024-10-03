// i2c.h

#ifndef I2C_HEADER
#define I2C_HEADER

void initI2CPins();
unsigned char i2cReadByteData( unsigned char devAddr, unsigned char regAddr );
void i2cReadBlockData( unsigned char devAddr, unsigned char regAddr,
                int n, unsigned char *result );
void i2cWriteByteData( unsigned char devAddr, unsigned char regAddr, unsigned char val);
void i2cWriteBlockData( char devAddr, unsigned char regAddr, unsigned char *val, int count );


#endif


