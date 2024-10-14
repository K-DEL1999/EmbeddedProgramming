#include "I2C_Slave_Header.h"

// Data Buffer
#define DATA_SIZE 256
static uint8_t data[DATA_SIZE];
static volatile uint32_t cnt;

static void init_gpio(void);

void app_main(void){
    init_gpio();

    /*for (; cnt < DATA_SIZE; cnt++){
        printf("%hu\n", data[cnt]);
    }*/

    while(1){
        I2C_await_request(data, DATA_SIZE); 
    }
}

static void IRAM_ATTR scl_ack_isr_handler(void* arg){
    // Needs testing
    gpio_intr_disable(I2C_SCL);
    
    if (rising_or_falling){
        time_to_set_data = 1;
        time_to_send_receive_data = 0;
        rising_or_falling = 0;
    }
    else {
        time_to_set_data = 0;
        time_to_send_receive_data = 1;
        rising_or_falling = 1;
    }
    
    gpio_intr_enable(I2C_SCL);
}

static void init_gpio(void){
    gpio_set_level(I2C_SDA, HIGH);
    gpio_set_level(I2C_SCL, HIGH);

    gpio_config_t io_conf = {};

    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_SDA_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;

    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_SCL_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;

    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(I2C_SCL, scl_ack_isr_handler, (void*)I2C_SCL);
    gpio_intr_disable(I2C_SCL);

    // Needs 250 micro seconds plus function calls to work properly
    // esp_sleep_enable_gpio_wakeup();
    // gpio_wakeup_enable(I2C_SDA, GPIO_INTR_LOW_LEVEL);
    // esp_light_sleep_start();
}

