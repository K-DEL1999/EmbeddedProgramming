#ifndef I2C_SLAVE_HEADER_H
#define I2C_SLAVE_HEADER_H

#include <unistd.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_sleep.h"

#define HIGH 1
#define LOW 0

#define DELAY 50

#define I2C_SDA 4// Data line
#define GPIO_SDA_PIN_SEL (1ULL << I2C_SDA)

#define I2C_SCL 16 // Clock line
#define GPIO_SCL_PIN_SEL (1ULL << I2C_SCL)

#define ADDRESS_FRAME_SIZE 8 // 8th bit for read/write
#define SLAVE_ADDRESS_READ 0b11111001
#define SLAVE_ADDRESS_WRITE 0b01111001

// extern from source
extern volatile uint8_t time_to_send_receive_data;
extern volatile uint8_t time_to_set_data;
extern volatile uint8_t rising_or_falling;

uint32_t I2C_await_request(uint8_t * data, uint32_t size);

#endif /*I2C_SLAVE_HEADER_H*/
