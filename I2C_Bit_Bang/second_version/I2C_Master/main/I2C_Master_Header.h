#ifndef I2C_MASTER_HEADER_H
#define I2C_MASTER_HEADER_H

#include <unistd.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"

#define HIGH 1
#define LOW 0

#define I2C_SDA 4// Data line
#define GPIO_SDA_PIN_SEL (1ULL << I2C_SDA)

#define I2C_SCL 16 // Clock line
#define GPIO_SCL_PIN_SEL (1ULL << I2C_SCL)

// Delays
#define DELAY 50
#define START_CONDITION_DELAY 150
#define SMALL_DELAY 25

// Externs from Source
extern volatile uint8_t master_clock;

// Flags from Source
extern volatile uint8_t time_to_send_receive_data;
extern volatile uint8_t time_to_update_clock;
extern volatile uint8_t cycle_in_progress;

// Externs from main
extern esp_timer_handle_t periodic_timer;

uint32_t I2C_transmit(uint8_t * data, uint32_t size);
uint32_t I2C_receive(uint8_t * data, uint32_t size);

#endif /*I2C_MASTER_HEADER_H*/
