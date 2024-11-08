#include "UART_Header.h"

// UART configuration struct
static uart_config_t uc;

// Delay time between bits -- determined after initialization
volatile uint32_t DELAY;

// Indexing variables
static volatile uint32_t index;
static volatile int cnt;

// Temp variables
static volatile uint32_t parity_bit;
static volatile uint32_t byte;
static volatile uint32_t bit;

// Flags
volatile uint32_t time_to_send_receive_data;

// Private functions --- for transmit
static void transmit_packet(void);
static inline void send_start_bit(void);
static inline void send_data_frame(void);
static inline void send_stop_bits(void);
static inline void send_data_bit(void);

// Private functions --- for receive
static void receive_packet(void);
static void receive_start_bit(void);
static void receive_data_frame(void);
static void receive_data_bit(uint8_t bit);
static void receive_stop_bits(void);

 
void configure_uart(uint32_t br, uint8_t dfs, uint8_t pb){
    uc.baud_rate = br;
    uc.data_frame_size = dfs;
    uc.parity_bit = pb;
    /* DELAY needs a cheap way to divide -- 1000000 / baud_rate*/
    /*SET DELAY HERE IN MICROSECONDS*/

    switch(br){
        case 9600:
            DELAY = BR_0;
            break;
        case 19200:
            DELAY = BR_1;
            break;
        case 38400:
            DELAY = BR_2;
            break;
        case 62500:
            DELAY = BR_3;
            break;
        case 125000:
            DELAY = BR_4;
            break;
        default:
            break;
    }
    
}

// Data for frame sizes 5 to 8 --- frame size of 9 will require special transmit
void uart_transmit(uint8_t * data, uint32_t size){
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    // Transmit false packet to get the corrupted packet out of the way
    transmit_packet();
 
    while (size){
        byte = *(data + index);
        transmit_packet();
        size--;
        index++;
    }
    
    index = 0;
    
    gptimer_stop(gptimer);
    time_to_send_receive_data = 0;
}

static void transmit_packet(void){
    send_start_bit();
    send_data_frame();
    send_stop_bits();

    /*
    while (cnt < (uc.data_frame_size + 2)){
        if (time_to_send_receive_data){
            if (cnt == 0){
                gpio_set_level(TX, LOW);
            }
            else if (cnt == uc.data_frame_size){
                gpio_set_level(TX, parity_bit);
            }
            else if (cnt == uc.data_frame_size+1){
                gpio_set_level(TX, HIGH);
            }
            else {
                gpio_set_level(TX, byte & 1);
                parity_bit ^= (byte & 1);
                byte >>= 1;
            }
            
            time_to_send_receive_data = 0;
            cnt++;
        }
    }

    cnt = 0;
    parity_bit = 0;
    */
}

static inline void send_start_bit(void){
    //esp_timer_start_periodic(periodic_timer, DELAY);
    //time_to_send_receive_data = 0;     
    //while (!time_to_send_receive_data){}
    // --------------------- WORKS ----------------------------//

    while (!time_to_send_receive_data){}
    gpio_set_level(TX, LOW);
    time_to_send_receive_data = 0;     
}

static inline void send_data_frame(void){
    while (cnt < uc.data_frame_size){
        if (time_to_send_receive_data){
            send_data_bit();
        }
    }

    if (uc.parity_bit){
        while (!time_to_send_receive_data){}
        gpio_set_level(TX, (parity_bit & 1));
        time_to_send_receive_data = 0;
    }
}

static inline void send_data_bit(void){
    bit = (byte >> cnt) & 1;
    gpio_set_level(TX, bit);
    parity_bit ^= bit;
    
    time_to_send_receive_data = 0;
    cnt++;
}

static inline void send_stop_bits(void){
    while (!time_to_send_receive_data){}
    time_to_send_receive_data = 0;
    gpio_set_level(TX, HIGH);
    
    // reset variables for use in next packet
    cnt = 0;
    parity_bit = 0;
    
    while (!time_to_send_receive_data){}
    time_to_send_receive_data = 0;

    //esp_timer_stop(periodic_timer);
}

void uart_receive(uint8_t * data, uint32_t size){
    while (size){
        receive_packet();
        *(data + index) = byte;         
        index++;
        size--;
        byte = 0;
    }

    index = 0;
}

static void receive_packet(void){
    receive_start_bit();
    receive_data_frame();
    receive_stop_bits();
}

static void receive_start_bit(void){
    while (gpio_get_level(RX)){}
    //esp_timer_start_periodic(periodic_timer, DELAY);
    gptimer_start(gptimer);
}

static void receive_data_frame(void){
    while (cnt < uc.data_frame_size){
        if (time_to_send_receive_data){
            receive_data_bit(gpio_get_level(RX));
        }
    }
    
    if (uc.parity_bit){
        while (!time_to_send_receive_data){}
        time_to_send_receive_data = 0;
        
        if (gpio_get_level(RX) != parity_bit){
            // Here you would log the corrupted packet and provide
            // some indication to the user to either check the logs
            // or that a corruption packet was received. You could 
            // also implement a way to let the transmitter know so 
            // that the packet is sent again.
        }
    }
}

static void receive_data_bit(uint8_t bit){
    time_to_send_receive_data = 0;
    byte |= (bit << cnt);
    cnt++;
    parity_bit ^= bit;
}
 
static void receive_stop_bits(){
    while (!time_to_send_receive_data){}
    time_to_send_receive_data = 0;
    //if (gpio_get_level(RX)){}//means stop bit was not LOW -- log ERROR}
 
    // reset variables for use in next packet
    cnt = 0;
    parity_bit = 0;

    while (!time_to_send_receive_data){}
    time_to_send_receive_data = 0;
    
    //esp_timer_stop(periodic_timer);
    gptimer_stop(gptimer);
    time_to_send_receive_data = 0;
}


