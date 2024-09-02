#include "SPI_Interface.h"

#define LOW 0
#define HIGH 1

static void init_master_gpio(void);
static void init_slave_gpio(void);
static void run_master(void);
static void run_slave(void);


void app_main(void)
{
    if (MASTER_OR_SLAVE){
        run_master();
    }    
    else {
        run_slave();
    }
}

static void run_master(void){
    init_master_gpio();
    init_master_spi();
    
    uint8_t data[1]; 
       
    data[0] = spi_master_receive_byte(); 
    if (SPIERRNO != 0){
        gpio_set_level(SPI_SCK, 1);
    }    

    init_master_spi();

    spi_master_transmit_byte(data, 1);
    if (SPIERRNO != 0){
        gpio_set_level(SPI_SCK, 1);
    }

    init_master_spi();

    while (1){}
}

static void run_slave(void){
    init_slave_gpio();
    init_slave_spi();

    uint8_t byte[2]; 
    byte[0] = 250;
    byte[1] = 0;

    while (1){
        while (gpio_get_level(SPI_CS) == HIGH){
            usleep(50);
        }
        
        if (gpio_get_level(SPI_MOSI)){ // Master is requesting data
            gpio_set_level(SPI_MISO, 1);
            
            spi_slave_transmit_byte(byte, 1);
        }
        else { // Master is sending data
            gpio_set_level(SPI_MISO, 0);
            byte[1] = spi_slave_receive_byte();

            if (byte[1] == 250){
                gpio_set_level(DATA_CONFIRMED, 1);
                usleep(10000000); // sleep for 10 seconds
                gpio_set_level(DATA_CONFIRMED, 0);
            }
        }

        init_slave_spi();
    }  
}

static void init_master_gpio(void){
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

static void init_slave_gpio(void){
     gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_OUTPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);   
}
