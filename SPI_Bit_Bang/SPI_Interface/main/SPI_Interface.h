#ifndef SPI_INTERFACE_H
#define SPI_INTERFACE_H

#include <unistd.h>
#include "driver/gpio.h"

// MASTER is 1, Slave is 0
#define MASTER_OR_SLAVE 1

// (polarity, phase)
#define SPI_MODE 0 // 0 -> (0,0), 1 -> (0,1), 2 -> (1,0), 3 -> (1,1)

//keep track of the clock 
#if SPI_MODE > 1
#define CLOCK 1
#else
#define CLOCK 0
#endif

#if MASTER_OR_SLAVE == 1
// Configure SPI pins -- clock, chip select, Output, Input
// Timer(SCK), Transmit(MOSI), Receive(MISO), Chip_Select(CS)
#define SPI_SCK 2
#define SPI_CS 0
#define SPI_MOSI 4     
#define GPIO_OUTPUT_PIN_SEL ((1ULL << SPI_SCK) | (1ULL << SPI_CS) | (1ULL << SPI_MOSI))

#define SPI_MISO 16
#define DATA_CONFIRMED 14
#define GPIO_INPUT_PIN_SEL (1ULL << SPI_MISO)
#else
#define SPI_SCK 13
#define SPI_CS 12
#define SPI_MOSI 14    
#define GPIO_INPUT_PIN_SEL ((1ULL << SPI_SCK) | (1ULL << SPI_CS) | (1ULL << SPI_MOSI))

#define SPI_MISO 27
#define DATA_CONFIRMED 2
#define GPIO_OUTPUT_PIN_SEL ((1ULL << SPI_MISO) | (1ULL << DATA_CONFIRMED))

#endif

// Defining this variable without extern in a header results in multiple defintions
// Wherever this header is called, a new copy of the defintion will be made 
// In this case main and spi_interface both called the header so 2 defintions were made
// volatile uint32_t SPIERRNO = 0x00000000; <-- WRONG
// Instead define in SPI_Interface.c and extern into header

extern volatile uint32_t SPIERRNO;

void init_master_spi(void);
void spi_master_transmit_byte(uint8_t * data, uint32_t size);
uint8_t spi_master_receive_byte(void);

void init_slave_spi(void);
void spi_slave_transmit_byte(uint8_t * data, uint32_t size);
uint8_t spi_slave_receive_byte(void);

#endif /* SPI_INTERFACE_H */

