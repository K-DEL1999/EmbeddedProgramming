#include "I2C_Master_Header.h"

// Clock
volatile uint8_t master_clock;

// Interrupt Flags
volatile uint8_t time_to_update_clock;
volatile uint8_t time_to_send_receive_data;
volatile uint8_t cycle_in_progress;

// Count variable
static volatile uint32_t cnt;
static volatile uint32_t index;
static volatile uint8_t byte;

// Slave variables
#define ADDRESS_FRAME_SIZE 8 // extra bit -- MSB -- is the r/w bit
#define SLAVE_ADDRESS_WRITE 0b01111001
#define SLAVE_ADDRESS_READ 0b11111001

// Private functions -- only for this file
static void wait_for_bus_idle_state(void);
static void start_condition(void);
static uint32_t send_address_frame(uint16_t address_frame);
static uint32_t send_data(uint8_t * data, uint32_t size);
static uint32_t receive_data(uint8_t * data, uint32_t size);
static void stop_condition(void);
static uint8_t await_ack(void);
static void send_ack(void);
static void reset_variables(void);

// Public Functions
uint32_t I2C_transmit(uint8_t * data, uint32_t size){
    wait_for_bus_idle_state();
    start_condition();
    if (send_address_frame(SLAVE_ADDRESS_WRITE)) return 1;
    if (send_data(data, size)) return 1;
    return 0;
}

uint32_t I2C_receive(uint8_t * data, uint32_t size){
    wait_for_bus_idle_state();
    start_condition();
    if (send_address_frame(SLAVE_ADDRESS_READ)) return 1;
    if (receive_data(data, size)) return 1;
    return 0;
}

static void wait_for_bus_idle_state(void){
        
    while (1){
        if (gpio_get_level(I2C_SDA)){
            if (gpio_get_level(I2C_SCL)){
                return;
            }
        } 
    }
}

static void start_condition(void){ 
    gpio_set_level(I2C_SDA, LOW);
    usleep(DELAY);
    gpio_set_level(I2C_SCL, LOW);
    usleep(START_CONDITION_DELAY);
}

static uint32_t send_address_frame(uint16_t address_frame){
    esp_timer_start_periodic(periodic_timer, DELAY);
    while (cnt < ADDRESS_FRAME_SIZE){ // plus 1 for the rw bit
        if (time_to_update_clock){
            if (time_to_send_receive_data){ // time_to_send_receive_data is set to 1 when clock is set to 1 
                gpio_set_level(I2C_SDA, ((address_frame >> cnt) & 1));
                cnt++;
                time_to_send_receive_data = 0; // reset for next clock high
            }

            gpio_set_level(I2C_SCL, master_clock);
            time_to_update_clock = 0;
        }
    }

    while (!time_to_update_clock){}
    time_to_update_clock = 0;
    gpio_set_level(I2C_SDA, HIGH);
    gpio_set_level(I2C_SCL, master_clock);
    
    cnt = 0;
    
    if (await_ack()){return 1;} 

    esp_timer_stop(periodic_timer);

    return 0;
}
static uint32_t send_data(uint8_t * data, uint32_t size){
    esp_timer_start_periodic(periodic_timer, DELAY);
    
    while (size){
        if (time_to_update_clock){
            if (cnt == 8){
                cnt = 0;
                size--;
                gpio_set_level(I2C_SDA, HIGH);
                if (await_ack()) return 1;
            }
            else {
                if (time_to_send_receive_data){
                    gpio_set_level(I2C_SDA, ((*(data + index) >> cnt) & 1));
                    cnt++;
                    time_to_send_receive_data = 0; // reset for next clock high
                    
                    if (cnt > 7){
                        index++;
                    }
                }     
                gpio_set_level(I2C_SCL, master_clock);
                time_to_update_clock = 0;
            }
        }
    }
 
    stop_condition();

    esp_timer_stop(periodic_timer);
    
    reset_variables();
    
    return 0;
}
static uint32_t receive_data(uint8_t * data, uint32_t size){
    esp_timer_start_periodic(periodic_timer, DELAY);
    
    while (size){
        if (time_to_update_clock){
            if (cnt == 8){
                if (size > 1){
                    send_ack();
                }
                size--;
                cnt = 0;
                *(data + index) = byte;
                index++;
                byte = 0;
            }
            else {
                if (time_to_send_receive_data){
                    byte |= (gpio_get_level(I2C_SDA) << cnt);
                    cnt++;
                    time_to_send_receive_data = 0;
                }

                gpio_set_level(I2C_SCL, master_clock);
                time_to_update_clock = 0;
            }
        }
    }
        
    stop_condition();

    esp_timer_stop(periodic_timer);
    
    reset_variables();

    return 0;
}

static void stop_condition(void){
    gpio_set_level(I2C_SDA, LOW);
    while (1){
        if (time_to_update_clock){
            gpio_set_level(I2C_SCL, master_clock);
            time_to_update_clock = 0;
            if (time_to_send_receive_data){
                
                while (time_to_send_receive_data){}
                
                gpio_set_level(I2C_SDA, HIGH);
                return;
            }
        }
    }     
}

static void reset_variables(void){
    // Clock
    master_clock = 0;

    // Interrupt Flags
    time_to_update_clock = 0;
    time_to_send_receive_data = 0;
    cycle_in_progress = 0;

    // Count variable
    cnt = 0;
    index = 0;
    byte = 0; 
}

static uint8_t await_ack(void){
    // When entering this block. the clock is on low
    // You have to wait until clock is high to read from SDA
    while (1){
        if (time_to_update_clock){
            gpio_set_level(I2C_SCL, master_clock);
            time_to_update_clock = 0;
            if (master_clock){
                if (!gpio_get_level(I2C_SDA)){
                    return 0; // if clock high and SDA low -- no error
                }
                else {
                    return 1; // otherwise error
                }
            }
        }
    }
}

static void send_ack(void){
    gpio_set_level(I2C_SCL, master_clock);
    time_to_update_clock = 0;
    
    gpio_set_level(I2C_SDA, LOW);
    
    while (1){
        if (time_to_update_clock){
            gpio_set_level(I2C_SCL, master_clock);
            time_to_update_clock = 0;
            
            if (!time_to_send_receive_data){
                gpio_set_level(I2C_SDA, HIGH);
                return;    
            }
        }
    }                
}
