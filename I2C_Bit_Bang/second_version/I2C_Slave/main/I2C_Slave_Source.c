#include "I2C_Slave_Header.h"

// Flags 
static volatile uint8_t read_or_write;
volatile uint8_t time_to_send_receive_data;
volatile uint8_t time_to_set_data;
volatile uint8_t rising_or_falling; // last edge seen -- set before use to sync flag with edge

// Count variable
static volatile uint32_t cnt;
static volatile uint8_t byte;
static volatile uint32_t index;
static volatile uint32_t data_size;

// Start condition
static volatile uint32_t stop_condition;

// Private functions
static uint32_t I2C_transmit(uint8_t * data, uint32_t size);
static uint32_t I2C_receive(uint8_t * data);
static void sleep_until_needed(void);
static void get_address(void);
static void send_ack(void);
static uint32_t await_ack(void);

// Private reset function
static void reset_variables(void);

// Check for stop condition
static void check_for_stop_condition(void);

uint32_t I2C_await_request(uint8_t * data, uint32_t size){
    sleep_until_needed();
    get_address();
    
    if (read_or_write == 2){return 1;} // No error -- not slave master looking for 

    // transmit returns success or fail status of transmission
    // receive returns size of data received
    return (read_or_write) ? I2C_transmit(data, size) : I2C_receive(data);
}

static uint32_t I2C_receive(uint8_t * data){
    rising_or_falling = gpio_get_level(I2C_SCL);
    gpio_intr_enable(I2C_SCL);
    while (!stop_condition){
        if (time_to_send_receive_data){
            byte |= (gpio_get_level(I2C_SDA) << cnt);
            cnt++;
            time_to_send_receive_data = 0;
            
            if ((cnt == 1) & (!gpio_get_level(I2C_SDA))){
                check_for_stop_condition();
            }        
        }
        if (time_to_set_data){
            if (cnt > 7){
                send_ack();
                *(data + index) = byte;
                index++;
                data_size++;
                cnt = 0;
                byte = 0;
            }
            
            time_to_set_data = 0; 
        }
    }

    
    gpio_intr_disable(I2C_SCL);
    reset_variables();   
 
    return data_size;
}

static uint32_t I2C_transmit(uint8_t * data, uint32_t size){
    rising_or_falling = gpio_get_level(I2C_SCL);
    gpio_intr_enable(I2C_SCL);
    while (!stop_condition){
        if (cnt == 8){ 
            index++;
            cnt = 0; 
            if (await_ack()){ 
                while (gpio_get_level(I2C_SDA)){}
                
                check_for_stop_condition();
                return 0;
            }
        }
        else {
            if (time_to_set_data){
                gpio_set_level(I2C_SDA, ((*(data+index) >> cnt) & 1 ));
                time_to_set_data = 0;
            }
            if (time_to_send_receive_data){
                cnt++;
                time_to_send_receive_data = 0;
            }    
        }
    }
    
    gpio_intr_disable(I2C_SCL);
    reset_variables();   
    
    return 0;
}

static void sleep_until_needed(void){
    while (1){
        if (!gpio_get_level(I2C_SDA)){ 
            if (!gpio_get_level(I2C_SCL)){
                gpio_wakeup_disable(I2C_SDA);
                return;
            }
        } 
    }
}

static void get_address(void){
    rising_or_falling = gpio_get_level(I2C_SCL);
    gpio_intr_enable(I2C_SCL);
    while (cnt < ADDRESS_FRAME_SIZE){
        if (time_to_send_receive_data){
            byte |= (gpio_get_level(I2C_SDA) << cnt);
            time_to_send_receive_data = 0;
            cnt++;
        }
        if (time_to_set_data){
            time_to_set_data = 0;
        }
    }

    if (byte == SLAVE_ADDRESS_WRITE){
        read_or_write = 0;
    }
    else if (byte == SLAVE_ADDRESS_READ){
        read_or_write = 1;
    }
    else { // Addresses did not match
        printf("Address: %hu\n--------------/", byte);
        read_or_write = 2;
        return;
    }

    send_ack();

    gpio_intr_disable(I2C_SCL);
    
    byte = 0;
    cnt = 0;
}

static void send_ack(void){
    while (1){
        if (time_to_set_data){
            gpio_set_level(I2C_SDA, LOW);
            time_to_set_data = 0;
        }
        if (time_to_send_receive_data){
            time_to_send_receive_data = 0;
        
            while (gpio_get_level(I2C_SCL)){}
            
            gpio_set_level(I2C_SDA, HIGH);
            return;
        }
    }  
}

static uint32_t await_ack(void){
    // Await ack will always be called on a SCL high
    while(1){
        if (time_to_set_data){
            gpio_set_level(I2C_SDA, HIGH);
            time_to_set_data = 0;
        }
        if (time_to_send_receive_data){
            time_to_send_receive_data = 0;
            if (!gpio_get_level(I2C_SDA)){
                return 0;
            }
            else {
                return 1;
            }
        }
    }
}

static void check_for_stop_condition(void){
    while (1){
        if (gpio_get_level(I2C_SDA)){
            stop_condition = 1;
            return;
        }
        if (time_to_set_data){
            return;
        }
    }
}

static void reset_variables(void){
    // Flags 
    read_or_write = 0;
    time_to_send_receive_data = 0;
    time_to_set_data = 0;
    rising_or_falling = 0; // most recent edge

    // Count variable
    cnt = 0;
    byte = 0;
    index = 0;
    data_size = 0;
    stop_condition = 0;    
}


