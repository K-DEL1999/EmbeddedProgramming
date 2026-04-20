# Bit-Banged I2C For ESP32 Microcontrollers

This project was my gate way into the world of embedded systems 


## Project Structure

- **I2C_Master_Source.c**: function defintions, helper function declarations, helper function defintions 
- **I2C_Master_Header.h**: function declarations and struct definitions
- **I2C_Master_Demo.c**: example showing how to use I2C Master driver functions
  
- **I2C_Slave_Source.c**: function defintions, helper function declarations, helper function definitions 
- **I2C_Slave_Header.h**: function declarations and struct defiintions
- **I2C_Slave_Demo.c**: example showing how to use I2C Slave driver functions

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

- **Can be used as an alternative to the built in ESP32 I2C peripheral**
- **Better understand the inner workings of the I2C protocol** 

