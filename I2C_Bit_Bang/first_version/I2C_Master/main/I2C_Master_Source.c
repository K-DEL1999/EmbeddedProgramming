#include "I2C_Master_Header.h"

// Controls Delay // 
#define DELAY 10
#define HALF_DELAY (DELAY >> 1) 
#define DOUBLE_DELAY (DELAY << 1)
#define START_CONDITION_DELAY 150
#define CHANGE_BUS_DRIVER_DELAY 65

#define SLAVE_ADDRESS_MR (1 << 7 | SLAVE_ADDRESS) // r/w bit is 1 -- MR : master read
#define SLAVE_ADDRESS_MW SLAVE_ADDRESS // r/w bit is 0 -- MW : master write

static uint32_t cnt;
   
static void start_condition(void);
static void stop_condition(void);

static void release_SDA_bus(void);
static void hold_SDA_bus(void);
static void release_SCL_bus(void);
static void hold_SCL_bus(void);

static void send_address_frame_mr(void);
static void send_address_frame_mw(void);
static uint8_t await_ack();

uint32_t I2C_transmit(uint8_t * data, uint32_t size){
    // Start Condition
    // But first check if I2C_SDA line is high to make sure no other master is 
    //  using the line.
    
    // while bus is busy -- we must wait for BUS_IDLE (HIGH-HIGH)
    while (!gpio_get_level(I2C_SDA) || !gpio_get_level(I2C_SCL)){}

    hold_SDA_bus();
    hold_SCL_bus();
    
    start_condition(); 
    send_address_frame_mw(); // You send the r/w bit along with the address frame

    release_SDA_bus();
    while (gpio_get_level(I2C_SDA)){}
 
    if (await_ack()){
        return 1; // No slave responded
    }         

    uint32_t index = 0;

    while (size){   
        
        hold_SDA_bus();
 
        for (cnt = 0; cnt < 8; cnt++){
            gpio_set_level(I2C_SDA, ((*(data + index) >> cnt) & 1));
            gpio_set_level(I2C_SCL, HIGH);
            usleep(DELAY);
            gpio_set_level(I2C_SCL, LOW);
            usleep(DELAY);
        }
        cnt = 0;

        release_SDA_bus();
        while (gpio_get_level(I2C_SDA)){}
        //usleep(CHANGE_BUS_DRIVER_DELAY);

        if (await_ack()){
            return 2; // Slave never responded to message while master transmitted data
        }

        index++;
        size--;
    }

    return 0;

    hold_SDA_bus();

    stop_condition();

    release_SDA_bus();
    release_SCL_bus();            

    return 0; // No errors when running 
}

static void start_condition(void){
    gpio_set_level(I2C_SDA, LOW);
    usleep(DELAY);
    gpio_set_level(I2C_SCL, LOW);
    usleep(START_CONDITION_DELAY);
}

static void stop_condition(void){
    gpio_set_level(I2C_SCL, HIGH);
    usleep(DELAY);
    gpio_set_level(I2C_SDA, HIGH);
    usleep(DELAY);
}

static void send_address_frame_mr(void){ // requesting to read from slave
    for (cnt = 0; cnt < SLAVE_ADDRESS_SIZE+1; cnt++){
        gpio_set_level(I2C_SDA, ((SLAVE_ADDRESS_MR >> cnt) & 1));
        gpio_set_level(I2C_SCL, HIGH);
        usleep(DELAY);
        gpio_set_level(I2C_SCL, LOW);
        usleep(DELAY);
    }
    cnt = 0;
}

static void send_address_frame_mw(void){ // requesting to write to slave
    for (cnt = 0; cnt < SLAVE_ADDRESS_SIZE+1; cnt++){
        gpio_set_level(I2C_SDA, ((SLAVE_ADDRESS_MW >> cnt) & 1));
        gpio_set_level(I2C_SCL, HIGH);
        usleep(DELAY);
        gpio_set_level(I2C_SCL, LOW);
        usleep(DELAY);
    }
    cnt = 0;
}

static uint8_t await_ack(void){
    gpio_set_level(I2C_SCL, HIGH);
    usleep(HALF_DELAY);
    if (gpio_get_level(I2C_SDA)){
        return 1; // Ack never received from 
    }
    usleep(HALF_DELAY);
    gpio_set_level(I2C_SCL, LOW);
    usleep(DELAY);   
    
    return 0; // no errors
}

static void release_SDA_bus(void){
    gpio_set_level(I2C_SDA, HIGH);
    gpio_set_direction(I2C_SDA, GPIO_MODE_INPUT);
}

static void hold_SDA_bus(void){
    gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(I2C_SDA, LOW);
}

static void release_SCL_bus(void){
    gpio_set_level(I2C_SCL, HIGH);
    gpio_set_direction(I2C_SCL, GPIO_MODE_INPUT);
}

static void hold_SCL_bus(void){
    gpio_set_direction(I2C_SCL, GPIO_MODE_OUTPUT_OD);
}

uint32_t I2C_receive(uint8_t * data){
    return 0;
}


