#include "I2C_Slave_Header.h"

void init_gpio(void);

void app_main(void)
{
    init_gpio();

    while (1){
        await_request(); 
    }
}

void init_gpio(void){
    // create gpio config struct
    gpio_config_t io_conf = {};
    
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);   
}
