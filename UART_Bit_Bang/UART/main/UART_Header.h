#ifndef UART_HEADER_H
#define UART_HEADER_H

#include <unistd.h>
#include <driver/gpio.h>
#include <driver/gptimer.h>
#include "esp_attr.h"

#define HIGH 1
#define LOW 0

// timer handler from main file
//extern esp_timer_handle_t periodic_timer;
extern gptimer_handle_t gptimer;

// flag from source for timer callback
extern volatile uint32_t time_to_send_receive_data;

// delay for timer
extern volatile uint32_t DELAY;

// Pins for transmitting and receiving
#define TX 17
#define GPIO_TX_PIN_SEL (1ULL << TX)
#define RX 5
#define GPIO_RX_PIN_SEL (1ULL << RX)

// Baud_rate delays -- corresponds to baud_rate delays in baud_rate_t enum
// 1/baud_rate
#define BR_0 104
#define BR_1 52
#define BR_2 26
#define BR_3 16
#define BR_4 8
// ---------------------------------------------- //
typedef enum {
    Baud_Rate_0 = 9600,
    Baud_Rate_1 = 19200,
    Baud_Rate_2 = 38400,
    Baud_Rate_3 = 62500,
    Baud_Rate_4 = 125000
} baud_rate_t;

typedef enum {
    frame_size_0 = 5,
    frame_size_1 = 6,
    frame_size_2 = 7,
    frame_size_3 = 8,
    frame_size_4 = 9
} data_frame_size_t;

typedef enum {
    ON = 1,
    OFF = 0
} parity_bit_t;

//Typedef defines a alias for an existing type
typedef struct {
    baud_rate_t baud_rate;
    data_frame_size_t data_frame_size;
    parity_bit_t parity_bit;
} uart_config_t;

void configure_uart(uint32_t br, uint8_t dfs, uint8_t pb);
void uart_transmit(uint8_t * data, uint32_t size);
void uart_receive(uint8_t * data, uint32_t size);

#endif /* UART_HEADER_H */
