// 2-gpio-test  main.c - demo the gpio pins


#include <unistd.h>
#include <stdio.h>
//#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "i2c.h"
#include "bmi270.h"





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

int itoc( int n, char *s, int i ) {
    if ( n < 0 ) {
        s[i++] = '-';
        i = itoc( -n, s, i );
    } else {
        if ( n >= 10 )
            i = itoc( n / 10, s, i );
        s[i++] = n % 10 + '0';
        s[i] = 0;
    }

    return i;
}

void uart_int( int v ) {
    char buffer[64];
    
    itoc( v, buffer, 0 );
    uart_puts( buffer );
}




int main() {
    delay(1000);
    
    uart_puts( "\n----------------------------------------\n\nWelcome\n\n" );
    
    uart_init();
    initI2CPins();
    
    uart_puts( "init done\n" );
    
    int error = bmi270Init( 0x69 );
    if ( error ) {
        uart_puts( "BMI270 not found\n" );
    } else {
        uart_puts( "BMI270 found\n" );
    }
    bmi270LoadConfigFile();
    
    bmi270SetMode( PERFORMANCE_MODE );
    bmi270SetAccRange( ACC_RANGE_2G );
    bmi270SetGyrRange( GYR_RANGE_1000 );
    bmi270SetAccOdr( ACC_ODR_200 );
    bmi270SetGyrOdr( GYR_ODR_200 );
    bmi270SetAccBwp( ACC_BWP_OSR4 );  
    bmi270SetGyrBwp( GYR_BWP_OSR4 );
    bmi270DisableFifoHeader();
    bmi270EnableDataStreaming();
    bmi270EnableAccFilterPerf();
    bmi270EnableGyrNoisePerf( );
    bmi270EnableGyrFilterPerf( );
    
    uart_puts( "\n\n" );
    
    int aVals[6];
    while( 1 ) {
        bmi270GetAllData( aVals );
        uart_puts( "Ax: " );
        uart_int( aVals[0] );
        uart_puts( "  Ay: " );
        uart_int( aVals[1] );
        uart_puts( "  Az: " );
        uart_int( aVals[2] );
        uart_puts( "  Gx: " );
        uart_int( aVals[3] );
        uart_puts( "  Gy: " );
        uart_int( aVals[4] );
        uart_puts( "  Gz: " );
        uart_int( aVals[5] );
        uart_puts( "\n" );
        
        
        delay( 10 );
    }

    while(1);
}



