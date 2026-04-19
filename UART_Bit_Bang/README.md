# Bit-Banged UART for ESP32 microcontrollers

A simple, instructional project demonstrating the underlying logic of the UART (Universal Asynchronous Receiver-Transmitter) protocol. UART operates with only 2 wires, TX being for transmitting data and RX for receiving data. With bit-banging each line is assigned a GPIO pin and initialized as an input or output pin depending on its purpose (receiving or transmitting data). Along with initializing GPIO pins a timer is required due to its asynchronous design. This project is meant to be used as an alternative to the built in UART peripheral most microcontrollers have while also serving the function of showing users how the protocol actually works.

## How to configure UART

<img width="515" height="120" alt="image" src="https://github.com/user-attachments/assets/7fb88286-703a-4b01-baf2-302344eb8ea1" />


