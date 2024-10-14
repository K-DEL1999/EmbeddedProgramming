#ifndef I2C_SLAVE_HEADER_H
#define I2C_SLAVE_HEADER_H

#include <unistd.h>
#include "driver/gpio.h"

#define HIGH 1
#define LOW 0

#define ADDRESS 91 // 1011011
#define ADDRESS_SIZE 7

#define I2C_SDA 26 // Serial Data Line 
#define I2C_SCL 25 // Clock Line
#define GPIO_INPUT_PIN_SEL ((1ULL << I2C_SCL) | (1ULL << I2C_SDA))


//--------------------------- //
//---- Debugging ------------ //
//--------------------------- //
#define I2C_TEST 2
#define GPIO_OUTPUT_TEST_PIN_SEL (1ULL << I2C_TEST)
//--------------------------- // 

uint32_t await_request(void);

// Declaring a function static in the header requires that it be defined in all files
//  in which the header is included.
/*
static void send_ack(void);
static void I2C_receive(uint8_t * data);
static void I2C_transmit(uint8_t * data, uint32_t size); 
*/

#endif /*I2C_SLAVE_HEADER_H*/
