#ifndef I2C_MASTER_HEADER_H
#define I2C_MASTER_HEADER_H

#include <unistd.h>
#include "driver/gpio.h"

#define HIGH 1
#define LOW 0

#define SLAVE_ADDRESS 91 //1011011
#define SLAVE_ADDRESS_SIZE 7 // can range from 7 - 10

#define I2C_SDA 4 // Data line
#define I2C_SCL 16 // Clock line
#define GPIO_INPUT_PIN_SEL ((1ULL << I2C_SCL) | (1ULL << I2C_SDA))

//--------------------------- //
//---- Debugging ------------ //
//--------------------------- //
#define I2C_CP_1 12
#define I2C_CP_2 14
#define I2C_CP_3 27
#define I2C_CP_4 26
#define GPIO_OUTPUT_TEST_PIN_SEL ((1ULL << I2C_CP_1)  | (1ULL << I2C_CP_2) | (1ULL << I2C_CP_3) | (1ULL << I2C_CP_4))
//--------------------------- // 

uint32_t I2C_transmit(uint8_t * data, uint32_t size);
uint32_t I2C_receive(uint8_t * data);

#endif /* I2C_MASTER_HEADER_H */

