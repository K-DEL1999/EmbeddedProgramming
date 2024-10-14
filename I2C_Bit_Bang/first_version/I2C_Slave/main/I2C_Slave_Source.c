#include "I2C_Slave_Header.h"

// Controls delay //
#define DELAY 10
#define HALF_DELAY (DELAY >> 1)
#define DOUBLE_DELAY (DELAY << 1)

#define DATA_SIZE 256
static uint8_t data[256];
static uint8_t byte;
static uint8_t cnt;
static uint32_t index;

static void send_ack(void);
static uint32_t I2C_receive(uint8_t * data);
static uint32_t I2C_transmit(uint8_t * data, uint32_t size); 

static void release_SDA_bus(void);
static void hold_SDA_bus(void);
static void wait_for_master_wakeup_signal(void);

uint32_t await_request(void){
    byte = 0;        

    wait_for_master_wakeup_signal(); 

    while (cnt < (ADDRESS_SIZE+1)){ // plus 1 for the R/W bit
        if (gpio_get_level(I2C_SCL)){
            byte |= (gpio_get_level(I2C_SDA) << cnt);
            cnt++;
            usleep(DELAY);
        } 
    } 

    if ((byte & 0x7F) != ADDRESS){
        return 1; // Not slave master is looking for
    }
   
    hold_SDA_bus();
    send_ack();
    release_SDA_bus();

    if (byte & 0x80){
        byte = 0;
        cnt = 0;
         
        I2C_transmit(data, DATA_SIZE);
    }
    else {
        byte = 0;
        cnt = 0;
    
        I2C_receive(data);
    }
    
    return 0; // No errors when running
}

static void send_ack(void){
    while(1){
        if (gpio_get_level(I2C_SCL)){
            gpio_set_level(I2C_SDA, LOW);
            usleep(DELAY);
            return;
        }
    }    
}

static void wait_for_master_wakeup_signal(void){
    while (1){
        if (!gpio_get_level(I2C_SDA) && !gpio_get_level(I2C_SCL)){
            return;
        }
    }
}

static void release_SDA_bus(void){
    gpio_set_level(I2C_SDA, HIGH);
    gpio_set_direction(I2C_SDA, GPIO_MODE_INPUT);
}

static void hold_SDA_bus(void){
    gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(I2C_SDA, LOW); 
}

uint32_t I2C_receive(uint8_t * data){
    while (1){
        if (gpio_get_level(I2C_SCL)){
            if (!gpio_get_level(I2C_SDA)){
                usleep(DELAY);
                if (gpio_get_level(I2C_SCL) && gpio_get_level(I2C_SDA)){    
                    index = 0;
                    return 0; // Stop condition signaled by master 
                }
            }
            else {
                byte |= (1 << cnt); 
            }
            cnt++;
        }

        if (cnt > 7){
            *(data+index) = byte;
            byte = 0;
            cnt = 0;
            index++;
            hold_SDA_bus();
            send_ack();
            release_SDA_bus();
        }
    }
}

uint32_t I2C_transmit(uint8_t * data, uint32_t size){
    return 0;
}

