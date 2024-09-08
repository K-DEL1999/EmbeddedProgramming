#include "SPI_Slave_Header.h"

// ErrorHandler
// Timer errors are in highest-order byte
// Transmit error (MOSI) are in the 2nd highest-order byte
// Receive  error (MISO) are in the 2nd lowest-order byte
// Chip Select error (MOSI) are in lowest-order byte
volatile uint32_t SPIERRNO = 0x00000000;
volatile uint32_t clock = CLOCK;
volatile uint32_t clockWatchDog;

void init_slave_spi(void){
    gpio_set_level(SPI_MISO, 0);
    gpio_set_level(DATA_CONFIRMED, 0);
}

void spi_slave_transmit(uint8_t * data, uint32_t size){
    uint8_t cnt = 0, index = 0;
    clockWatchDog = 0;

    gpio_set_level(SPI_MISO, 1);

    while (!gpio_get_level(SPI_CS)){
        if (clock == CLOCK && gpio_get_level(SPI_SCK) == !CLOCK){
            gpio_set_level(SPI_MISO, ((((*(data + index)) >> cnt) & 1)));
            cnt++;
            if (cnt == 8){
                cnt = 0;
                index++;
            }
            clock = !CLOCK;
            clockWatchDog = 0;
        }

        if (gpio_get_level(SPI_SCK) == CLOCK) { 
            clock = CLOCK; 
        }
        
        clockWatchDog++;
        if (clockWatchDog > 100000){
            SPIERRNO |= (1 << 1);
            return;
        }

    }
}

void spi_slave_receive(uint8_t * data){
    uint8_t byte = 0, cnt = 0, index = 0; 
    clockWatchDog = 0;

    gpio_set_level(SPI_MISO, 0);

    while (!gpio_get_level(SPI_CS)){
        if (clock == CLOCK && gpio_get_level(SPI_SCK) == !CLOCK){
            usleep(5);
            byte |= (gpio_get_level(SPI_MOSI) << cnt);
            cnt++;
            if (cnt == 8){
                *(data + index) = byte;
                index++;
                cnt = 0;
                byte = 0;
            }
            clock = !CLOCK;
            clockWatchDog = 0;
        }

        if (gpio_get_level(SPI_SCK) == CLOCK) { 
            clock = CLOCK; 
        }
        
        clockWatchDog++;
        if (clockWatchDog > 100000){
            SPIERRNO |= 1;
            return;
        }
    }
}

