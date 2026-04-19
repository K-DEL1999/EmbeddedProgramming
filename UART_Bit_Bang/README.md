# Bit-Banged UART for ESP32 microcontrollers

A simple, instructional project demonstrating the underlying logic of the UART (Universal Asynchronous Receiver-Transmitter) protocol. UART operates with only 2 wires, TX being for transmitting data and RX for receiving data. With bit-banging each line is assigned a GPIO pin and initialized as an input or output pin depending on its purpose (receiving or transmitting data). Along with initializing GPIO pins a timer is required due to its asynchronous design. This project is meant to be used as an alternative to the built in UART peripheral most microcontrollers have while also serving the function of showing users how the protocol actually works.

## Project Structure

- **UART_Source.c**: function defintions, helper function declarations, helper function declarations 
- **UART_Header.h**: function declarations and struct defintions
- **UART_demo.c**: example showing how to use UART functions

## Available UART Functions 

```c
void configure_uart(uint32_t br, uint8_t dfs, uint8_t pb);
void uart_transmit(uint8_t * data, uint32_t size);
void uart_receive(uint8_t * data, uint32_t size);
```
Users are provided a configuration function, transmit function and a receive function. 

## How to configure UART

UART is configured depending on the configuration functions arguments. Users can set the **baud rate**, **data frame size**, and **polarity**. Values are limited to the predefined values set in the following enumeratiions.

Baud rate 
```c
typedef enum {
    Baud_Rate_0 = 9600,
    Baud_Rate_1 = 19200,
    Baud_Rate_2 = 38400,
    Baud_Rate_3 = 62500,
    Baud_Rate_4 = 125000
} baud_rate_t;
```

Data frame size
```c
typedef enum {
    frame_size_0 = 5,
    frame_size_1 = 6,
    frame_size_2 = 7,
    frame_size_3 = 8,
    frame_size_4 = 9
} data_frame_size_t;
```

Polarity bit
```c
typedef enum {
    ON = 1,
    OFF = 0
} parity_bit_t;
```




















