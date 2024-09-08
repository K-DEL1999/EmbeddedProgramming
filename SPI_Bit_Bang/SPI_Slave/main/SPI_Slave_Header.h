#ifndef SPI_SLAVE_HEADER_H
#define SPI_SLAVE_HEADER_H

#include <unistd.h>
#include "driver/gpio.h"

//Array Size for debugging
#define ARRAY_SIZE 256

#define CLOCK 0

#define SPI_SCK 13
#define SPI_CS 12
#define SPI_MOSI 14    
#define GPIO_INPUT_PIN_SEL ((1ULL << SPI_SCK) | (1ULL << SPI_CS) | (1ULL << SPI_MOSI))

#define SPI_MISO 27
#define DATA_CONFIRMED 2
#define GPIO_OUTPUT_PIN_SEL ((1ULL << SPI_MISO) | (1ULL << DATA_CONFIRMED))

extern volatile uint32_t SPIERRNO;

void init_slave_spi(void);
void spi_slave_transmit(uint8_t * data, uint32_t size);
void spi_slave_receive(uint8_t * data);


#endif /*SPI_SLAVE_HEADER_H*/
