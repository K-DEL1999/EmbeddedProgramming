#include <DS18B20_Driver_Header.h>

void init_gpio(void);

void app_main(void)
{
    init_gpio();
   
    /* 
    if (!search_rom()){
        usleep(500);
        gpio_set_level(DQ, LOW);
        usleep(50); 
        gpio_set_level(DQ, HIGH);
    }*/ 

    search_rom();
    match_rom(device_ROM_numbers[0]);
    read_scratchpad();

    usleep(500);

    match_rom(device_ROM_numbers[0]);
    set_config(2);
    write_scratchpad();
    
    usleep(500);

    match_rom(device_ROM_numbers[0]);
    read_scratchpad();

    while (1){}
}

void init_gpio(void){
    gpio_set_level(DQ, HIGH);
    
    gpio_config_t io_conf = {};

    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_DQ_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;

    gpio_config(&io_conf);

}
