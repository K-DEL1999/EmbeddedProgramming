#include "SPI_Master_Header.h"
#include "esp_timer.h"

static uint8_t data[ARRAY_SIZE];

static volatile uint64_t receive_sum;
static volatile uint64_t transmit_sum;
static volatile uint8_t cnt;

// Transmit timing
static volatile uint64_t start_time_tx;
static volatile uint64_t end_time_tx;

// Revieve timing
static volatile uint64_t start_time_rx;
static volatile uint64_t end_time_rx;

static void init_master_gpio(void);

void app_main(void){
    init_master_gpio();
    init_master_spi();

    

    while (1){
        start_time_rx = esp_timer_get_time(); 
        spi_master_receive(data);
        end_time_rx = esp_timer_get_time(); 
        
        receive_sum += end_time_rx - start_time_rx; 
        init_master_spi();
         
        usleep(10);

        start_time_tx = esp_timer_get_time(); 
        spi_master_transmit(data, ARRAY_SIZE);
        end_time_tx = esp_timer_get_time();

        transmit_sum += end_time_tx - start_time_tx; 
        init_master_spi();

        if ((cnt % 16) == 0){
            printf("curr_transmit_avg = %lld\n", transmit_sum >> 4);
            printf("curr_recieve_avg = %lld\n", receive_sum >> 4);
            printf("\n\n");
            cnt = 0;
            receive_sum = 0;
            transmit_sum = 0;    
        }
        cnt++;
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

