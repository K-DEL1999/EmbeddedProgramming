# Bit-Banged SPI For ESP32 Microcontrollers

This project is intended for educational purposes but can serve as an alterantive to the native SPI peripheral most microcontrollers house. SPI (Serial Peripheral Interface) is a synchronous protocol, meaning a clock is shared amongst all connected devices, that requires 4 lines. Of course there are variations of SPI which may require more or less than 4 lines but the main concept remains unchanged - send and receive bits on rising edge or falling edge. SPI can be full duplex (transmit and receive simultaneously) or half duplex (transmit and receive seperately). This driver implements a half duplex SPI. The lines in SPI are CLK, MOSI, MISO and CS. CLK is clock, MOSI is data out, MISO is data in and CS selects the receiver. This bit banged implementation of SPI will allow users to understand how SPI works while also allowing them to connect to modules via SPI

## Project Structure

### Master Driver
- **SPI_Master_Source.c**: function defintions, helper function declarations, helper function defintions 
- **SPI_Master_Header.h**: function declarations and struct definitions
- **SPI_Master_Demo.c**: example showing how to use SPI Master driver functions

### Slave Driver
- **SPI_Slave_Source.c**: function defintions, helper function declarations, helper function definitions 
- **SPI_Slave_Header.h**: function declarations and struct defiintions
- **SPI_Slave_Demo.c**: example showing how to use SPI Slave driver functions

## Available SPI Master Functions 
 ```c
void init_master_spi(void);
void spi_master_transmit(uint8_t * data, uint32_t size);
void spi_master_receive(uint8_t * data);
```
Users are provided an intialization function, transmit function and a receive function. 

## Available SPI Slave Functions 
```c
void init_slave_spi(void);
void spi_slave_transmit(uint8_t * data, uint32_t size);
void spi_slave_receive(uint8_t * data);
```
Users are provided an intialization function, transmit function and a receive function. 

## Setting the GPIO pins

To set the GPIO pin replace <PIN_NUMBER> with the desired pin number. Setting the pins for Master and Slave are the same. 


MOSI - data out  
MISO - data in  
CS - chip select  
SCK - clock   

```c
#define SPI_SCK <PIN_NUMBER>
#define SPI_CS <PIN_NUMBER>
#define SPI_MOSI <PIN_NUMBER>     
#define GPIO_OUTPUT_PIN_SEL ((1ULL << SPI_SCK) | (1ULL << SPI_CS) | (1ULL << SPI_MOSI))

#define SPI_MISO <PIN_NUMBER>
#define DATA_CONFIRMED 14
#define GPIO_INPUT_PIN_SEL (1ULL << SPI_MISO)
```

## SPI Initialization

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



