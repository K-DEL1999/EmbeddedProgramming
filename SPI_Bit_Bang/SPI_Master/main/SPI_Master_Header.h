#ifndef SPI_MASTER_HEADER_H
#define SPI_MASTER_HEADER_H

#include <unistd.h>
#include "driver/gpio.h"

//Array Size for debugging
#define ARRAY_SIZE 256

#define CLOCK 0

#define SPI_SCK 2
#define SPI_CS 0
#define SPI_MOSI 4     
#define GPIO_OUTPUT_PIN_SEL ((1ULL << SPI_SCK) | (1ULL << SPI_CS) | (1ULL << SPI_MOSI))

#define SPI_MISO 16
#define DATA_CONFIRMED 14
#define GPIO_INPUT_PIN_SEL (1ULL << SPI_MISO)

extern volatile uint32_t SPIERRNO;

void init_master_spi(void);
void spi_master_transmit(uint8_t * data, uint32_t size);
void spi_master_receive(uint8_t * data);

#endif /*SPI_MASTER_HEADER_H*/


