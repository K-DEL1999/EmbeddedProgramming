#include "SPI_Slave_Header.h"

static uint8_t data_rx[ARRAY_SIZE];
static uint8_t data_tx[ARRAY_SIZE];

static void init_slave_gpio(void);
static void generate_data(void);
static void verify_data(void);

void app_main(void){
    
    init_slave_gpio();
    init_slave_spi();

    RESTART_FROM_TRANSMIT:
        usleep(4000000);
        init_slave_spi();

    RESTART_FROM_RECEIVE:
        usleep(7000000);
        init_slave_spi();

    generate_data();
        
    while (1){
        while (gpio_get_level(SPI_CS) == 1){}       
 
        if (gpio_get_level(SPI_MOSI)){ // Master is requesting data
            spi_slave_transmit(data_tx, ARRAY_SIZE);
            if (SPIERRNO != 0){
                goto RESTART_FROM_TRANSMIT;
            }
        }
        else { // Master is sending data
            spi_slave_receive(data_rx);
            if (SPIERRNO != 0){
                goto RESTART_FROM_RECEIVE;
            }
            
            verify_data();
        }

        init_slave_spi();       
    }
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

static void generate_data(void){
    for (int i = 0; i < ARRAY_SIZE; i++){
        data_tx[i] = i;
    } 
}

static void verify_data(void){
    for (int i = 0; i < ARRAY_SIZE; i++){
        if (data_rx[i] != data_tx[i]){
            goto NOT_A_MATCH; 
        }
    }

    goto SUCCESS;

    NOT_A_MATCH:
        gpio_set_level(DATA_CONFIRMED, 1);
        usleep(2000000);
        gpio_set_level(DATA_CONFIRMED, 0);
        usleep(2000000);
        gpio_set_level(DATA_CONFIRMED, 1);
        usleep(2000000);

    SUCCESS:
        gpio_set_level(DATA_CONFIRMED, 1);
        usleep(1000000); // sleep for 1 seconds
        gpio_set_level(DATA_CONFIRMED, 0);
} 
