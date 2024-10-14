#include "I2C_Master_Header.h"

void init_gpio(void);

#define DATA_SIZE 256
static uint8_t data[256];

void app_main(void)
{
    init_gpio();
    
    for (int i = 0; i < DATA_SIZE; i++){
        data[i] = i;
    }
    
    I2C_transmit(data, DATA_SIZE);

    while (1){

    }
}

void init_gpio(void){
    // create gpio config struct
    gpio_config_t io_conf = {};

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = GPIO_OUTPUT_TEST_PIN_SEL;
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);

    gpio_set_level(I2C_CP_1, LOW);
    gpio_set_level(I2C_CP_2, LOW);
    gpio_set_level(I2C_CP_3, LOW);
    gpio_set_level(I2C_CP_4, LOW);
}

