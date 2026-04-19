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
Users are provided a configuration function, transmit function and a receive function. UART is configured depending on the arguments provided in the configuration function. 

## How to configure UART

The 



