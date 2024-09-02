#include "SPI_Interface.h"

// ErrorHandler
// Timer errors are in highest-order byte
// Transmit error (MOSI) are in the 2nd highest-order byte
// Receive  error (MISO) are in the 2nd lowest-order byte
// Chip Select error (MOSI) are in lowest-order byte
volatile uint32_t SPIERRNO = 0x00000000;
volatile uint32_t clock = CLOCK;

//---------------------------------------------------------------------------------------
// MASTER FUNCTIONS
//---------------------------------------------------------------------------------------

void init_master_spi(void){
    gpio_set_level(SPI_SCK, CLOCK);
    gpio_set_level(SPI_CS, 1);
    gpio_set_level(SPI_MOSI, 0);    
    usleep(2000000);        
}

void spi_master_transmit_byte(uint8_t * data, uint32_t size){

    gpio_set_level(SPI_CS, 0); // 0 indicates that someone is occupy/controlling the bus
    gpio_set_level(SPI_MOSI, 0); // Slave reads MOSI and if low, master will transmit data
   
    usleep(2000000); // wait 2 second for SPI_MISO to respond
    
    if (gpio_get_level(SPI_MISO) != 0){ // Checks to see if slave returns same state as MOSI
            SPIERRNO |= (1 << 4); // transmit error <-- slave did not respond
            return;
    }

    while (size){       

        if ( SPI_MODE == 0 || SPI_MODE == 2){
            for (uint32_t cnt = 0; cnt < 8; cnt++){
                gpio_set_level(SPI_MOSI, (*data & (1 << cnt)) ); 

                clock = !clock;
                gpio_set_level(SPI_SCK, clock);
                
                usleep(2000000); // wait 2 second

                clock = !clock;
                gpio_set_level(SPI_SCK, clock);
                
                usleep(2000000); // wait 2 second
            }
        }
        else {
            for (uint32_t cnt = 0; cnt < 8; cnt++){
                clock = !clock;
                gpio_set_level(SPI_SCK, clock);
                        
                // Timer can substitue -- holds gpio state for 1/2 ms
                //for (uint32_t i = 0; i < 100; i++){} 
                usleep(2000000); // wait 2 second

                clock = !clock;
                gpio_set_level(SPI_SCK, clock);
                
                gpio_set_level(SPI_MOSI, (*data & (1 << cnt)) ); 

                // Timer can substitue -- holds gpio state for 1/2 ms
                //for (uint32_t i = 0; i < 100; i++){}
                usleep(2000000); // wait 2 second
            }
        }

        size--;
    }
   
    // Reset clock variables 
    clock = CLOCK;
}

uint8_t spi_master_receive_byte(void){ // returns number of bytes of data
    uint32_t byte = 0;

    gpio_set_level(SPI_CS, 0); // 0 indicates that someone is occupy/controlling the bus
    gpio_set_level(SPI_MOSI, 1); // slave reads MOSI and if high, master is requesting data   

    usleep(2000000); // wait 2 second for SPI_MISO to respond
    
    // Slave will indicate that it is ready by repsonding with the state of the MOSI
    if (gpio_get_level(SPI_MISO) != 1){ // Check if MOSI == MISO
        SPIERRNO |= (1 << 4); // transmit error <-- slave did not respond
        return 0;
    }

    if ( SPI_MODE == 0 || SPI_MODE == 2){
        for (uint32_t cnt = 0; cnt < 8; cnt++){

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);
                            
            usleep(2000000); // wait for 2 seconds
           
            byte |= (gpio_get_level(SPI_MISO) << cnt); 
            
            clock = !clock;
            gpio_set_level(SPI_SCK, clock); 
            
            usleep(2000000); // wait for 2 seconds
        }
    }
    else {
        for (uint32_t cnt = 0; cnt < 8; cnt++){
            clock = !clock;
            gpio_set_level(SPI_SCK, clock);       
            
            usleep(2000000); // wait for 2 seconds
            
            clock = !clock;
            gpio_set_level(SPI_SCK, clock);        

            byte |= (gpio_get_level(SPI_MISO) << cnt); 
            
            // Timer can substitue -- holds gpio state for 1 second
            // for (uint32_t i = 0; i < 100; i++){}
            usleep(2000000); // wait for 2 seconds
        }
    }

    gpio_set_level(SPI_CS, 1); // 1 indicates that the bus has been released
    clock = CLOCK;
    
    return byte;
}

//---------------------------------------------------------------------------------------
// SLAVE FUNCTIONS
//---------------------------------------------------------------------------------------


void init_slave_spi(void){
    gpio_set_level(SPI_MISO, 0);
    gpio_set_level(DATA_CONFIRMED, 0);    
    usleep(2000000);    
}

void spi_slave_transmit_byte(uint8_t * data, uint32_t size){
    uint8_t cnt = 0;

    if (SPI_MODE == 0 || SPI_MODE == 2){
        while (!gpio_get_level(SPI_CS)){
            if (clock == CLOCK && gpio_get_level(SPI_SCK) == !CLOCK){
                gpio_set_level(SPI_MISO, (*data & (1 << cnt)));
                cnt++;
                clock = !CLOCK;     
            }
            
            if (gpio_get_level(SPI_SCK) == CLOCK) { clock = CLOCK; };
               
        }
    }
    else {
    }
}

uint8_t spi_slave_receive_byte(void){
    uint8_t byte = 0, cnt = 0;

    if (SPI_MODE == 0 || SPI_MODE == 2){
        while (!gpio_get_level(SPI_CS)){
            if (clock == CLOCK && gpio_get_level(SPI_SCK) == !CLOCK){
                usleep(1000000);
                byte |= (gpio_get_level(SPI_MOSI) << cnt);
                cnt++;
                clock = !CLOCK;
            }

            if (gpio_get_level(SPI_SCK) == CLOCK) { clock = CLOCK; };                  
        }    
    }
    else {
    
    }

    
    return byte;
}


