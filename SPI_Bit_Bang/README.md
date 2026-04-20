# Bit-Banged UART For ESP32 Microcontrollers

This project is intended for educational purposes but can serve as an alterantive to the native SPI peripheral most microcontrollers house. SPI (Serial Peripheral Interface) is a synchronous protocol, meaning a clock is shared amongst all connected devices, that requires 4 lines. Of course there are variations of SPI which may require more or less than 4 lines but the main concept remains unchanged - send and receive bits on rising edge or falling edge. SPI can be full duplex (transmit and receive simultaneously) or half duplex (transmit and receive seperately). This driver implements a half duplex SPI. The lines in SPI are CLK, MOSI, MISO and CS. CLK is clock, MOSI is data out, MISO is data in and CS selects the receiver. This bit banged implementation of SPI will allow users to understand how SPI works while also allowing them to connect to modules via SPI

## Project Structure

- **SPI_Master_Source.c**: function defintions, helper function declarations, helper function defintions 
- **SPI_Master_Header.h**: function declarations and struct definitions
- **SPI_Master_Demo.c**: example showing how to use SPI Master driver functions

- **SPI_Slave_Source.c**: function defintions, helper function declarations, helper function definitions 
- **SPI_Slave_Header.h**: function declarations and struct defiintions
- **SPI_Slave_Demo.c**: example showing how to use SPI Slave driver functions

## Available SPI Master Functions 

## Setting the GPIO pins

## How To Configure UART

## Data Transmission and Data Reception

## How to Use

1. Ensure you have a working ESP32 development environment (this project was designed using the ESP-IDF development framework)
2. Clone the repository
3. Change the `TX` and `RX` pin numbers in the `UART_Header.h` file to pins you wish to use
4. Connect devices - connect TX pin in one device to the RX pin in the other and vice versa
5. Build and Flash

### If using ESP-IDF
```sh
idf.py build
idf.py flash
```

## Application

- **Can be used as an alternative to the built in ESP32 UART peripheral**
- **Better understand the inner workings of the UART protocol** 



