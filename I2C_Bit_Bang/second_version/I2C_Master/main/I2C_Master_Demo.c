#include "I2C_Master_Header.h"

// time for stats
static volatile uint64_t start_time;
static volatile uint64_t end_time;

// Data
#define DATA_SIZE 256
static uint8_t data_tx[DATA_SIZE];
static uint8_t data_rx[DATA_SIZE];
static volatile uint32_t cnt;

// Create timer handler 
esp_timer_handle_t periodic_timer;

// Private functions
static void init_gpio(void);
static void init_timer(void);
static void periodic_timer_callback(void* arg);

void app_main(void){
    init_gpio();
    init_timer();

    for (; cnt < DATA_SIZE; cnt++){
        data_tx[cnt] = cnt;
    }
    cnt = 0;
   
    start_time = esp_timer_get_time(); 
    I2C_transmit(data_tx, DATA_SIZE);
    end_time = esp_timer_get_time();

    printf("\n--------------------\n");
    printf("Transmit Time: %llu\n", end_time - start_time); 
    printf("--------------------\n");

    usleep(1000); 
    
    start_time = esp_timer_get_time(); 
    I2C_receive(data_rx, DATA_SIZE);
    end_time = esp_timer_get_time();

    printf("--------------------\n");
    printf("Receive Time: %llu\n", end_time - start_time); 
    printf("--------------------\n");
    
    
    
    for (; cnt < DATA_SIZE; cnt++){
        printf("%hu\n", data_rx[cnt]);
    }
 
    while(1){
       usleep(1);
    }
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

    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_SCL_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;


    gpio_config(&io_conf);
}

static void init_timer(void){
    // Initialize timer struct
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback
    };

    // Assign handler to timer struct
    esp_timer_create(&periodic_timer_args, &periodic_timer);
}

static void periodic_timer_callback(void* arg){
    esp_timer_stop(periodic_timer);
    
    time_to_update_clock = 1;
    
    if (master_clock){
        master_clock = 0;
        time_to_send_receive_data = 0; // bug if this line is not added -- first wave is missing
    }
    else {
        master_clock = 1;
        time_to_send_receive_data = 1;
    }
    
    esp_timer_start_periodic(periodic_timer, DELAY);
}



