#include "UART_Header.h"

//esp_timer_handle_t periodic_timer;
gptimer_handle_t gptimer;

#define DATA_SIZE 256
uint8_t data[DATA_SIZE];

// Private functions
//static void init_timer(void);
static void init_gpio(void);
static void init_gptimer(void);
//static void periodic_timer_callback(void * arg);

void app_main(void)
{
    //init_timer();
    init_gpio();
    configure_uart(Baud_Rate_4, frame_size_3, ON);
    init_gptimer();

    // TRANSMIT
    
    for (int i = 0; i < DATA_SIZE; i++){
        data[i] = i;
    }  

    uart_transmit(data, DATA_SIZE); 
    uart_receive(data, DATA_SIZE);
    

    // RECEIVE
    /* 
    uart_receive(data, 256);
    
    for (int i = 0; i < 256; i++){
        data[i] = 255 - data[i];
    }

    usleep(50);

    uart_transmit(data, 256); 
    */
    
    while (1){}
}



void init_gpio(void){
    gpio_set_level(TX, HIGH);
    gpio_config_t io_conf = {};

    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_TX_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;

    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_RX_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;

    gpio_config(&io_conf);
}

// Software periodic timer -- caps out at 50us delay time
// For delays smaller than 50us you have to use a hardware timer -- gptimer
/*void init_timer(void){
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        // If you wanted to include arguments you would do so in this line
        .arg = (void *) periodic_timer
    };
    esp_timer_create(&periodic_timer_args, &periodic_timer);
}*/

static bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
    time_to_send_receive_data = 1;
    return 0;
}

// Hardware timer
void init_gptimer(void){
    // Creates general purpose timer handler
    //gptimer_handle_t gptimer = NULL;

    // Creates timer config
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };

    // Creates timer
    gptimer_new_timer(&timer_config, &gptimer);

    // Sets the period -- the alarm
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = DELAY, // period = 1s --- default is 1 000 000 us which is 1 s
        .flags.auto_reload_on_alarm = true,
    };

    // Sets the alarm action
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // Creates gptimer callback
    gptimer_event_callbacks_t cbs = {
        .on_alarm = gptimer_callback,
    };
    
    // Registers the callback function
    gptimer_register_event_callbacks(gptimer, &cbs, (void *) gptimer);

    // Enables the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    /*
    // Starts the timer
    gptimer_start(gptimer);
    
    // Stop the timer
    gptimer_stop(gptimer);*/
}

/*
static void periodic_timer_callback(void * arg){
    time_to_send_receive_data = 1;
}*/


