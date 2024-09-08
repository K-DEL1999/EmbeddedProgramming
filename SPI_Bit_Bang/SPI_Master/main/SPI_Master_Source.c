#include "SPI_Master_Header.h"

// ErrorHandler
// Cannot connect to slave when transmitting 0x00000001
// Cannot connect to slave when receiving 0x00000010
volatile uint32_t SPIERRNO = 0x00000000;
volatile uint32_t clock = CLOCK;

void init_master_spi(void){
    gpio_set_level(SPI_SCK, CLOCK);
    gpio_set_level(SPI_CS, 1);
    gpio_set_level(SPI_MOSI, 0);
}


void spi_master_transmit(uint8_t * data, uint32_t size){
    uint32_t index = 0;

    gpio_set_level(SPI_CS, 0); // 0 indicates that someone is occupy/controlling the bus
    gpio_set_level(SPI_MOSI, 0); // Slave reads MOSI and if low, master will transmit data
    
    // Wait for MISO confirmation --- TEST -------------------
    // Slave will indicate that it is ready by repsonding with the state of the MOSI
    while (gpio_get_level(SPI_MISO) != 0){usleep(1);} 
    // ------------------------------------------------------    

    while (size){
        for (uint32_t cnt = 0; cnt < 8; cnt++){
            gpio_set_level(SPI_MOSI, (*(data + index) & (1 << cnt)) );

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10);

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10); 
        }
      
        index++;  
        size--;
    }

    // Reset clock variables 
    clock = CLOCK;

}


void spi_master_receive(uint8_t * data){ // returns number of bytes of data
    uint32_t byte = 0, index = 0, size = ARRAY_SIZE;

    gpio_set_level(SPI_CS, 0); // 0 indicates that someone is occupy/controlling the bus
    gpio_set_level(SPI_MOSI, 1); // slave reads MOSI and if high, master is requesting data   

    // Wait for MISO confirmation --- TEST -------------------
    // Slave will indicate that it is ready by returning the state of the MOSI
    while (gpio_get_level(SPI_MISO) != 1){usleep(1);} 
    // ------------------------------------------------------

    while(size){
        byte = 0;
        
        for (uint32_t cnt = 0; cnt < 8; cnt++){

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10); 

            byte |= (gpio_get_level(SPI_MISO) << cnt);

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10); 
        }

        *(data + index) = byte;
        index++;
        size--;
    }
    
    clock = CLOCK;

}


