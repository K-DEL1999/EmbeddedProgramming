# DS18B20 Driver For ESP32 Microcontrollers

This is a driver for the DS18B20 temperature sensor written in C. It enables communcation between an ESP32 microcontroller and multiple DS18B20 temperature sensors. The means of communication is through a 1 wire protocol unique to this sensor.

## Project Structure

- **DS18B20_Source.c**: function defintions, helper function declarations, helper function defintions 
- **DS18B20_Header.h**: function declarations and struct definitions
- **DS18B20_Demo.c**: example showing how to use SPI Master driver functions

## Available Functions 

### Low Level Functions

These are the functions that send commands to the sensor using the 1 wire protocol. Using these you can create your own high level functions to obtain the temperature reading from a device.

```c
// ROM cmds
bool search_rom(void);
bool read_rom(void);
bool skip_rom(void);
bool match_rom(uint64_t rom_no);
bool alarm_search(void);

// Memory cmds
void write_scratchpad(void);
void read_scratchpad(void);
void copy_scratchpad(void);
void convert_temp(void);
void recall_E2(void);
bool read_power_supply(void);
```

### High Level Functions

These are provided in case a user does not wish to implement their own functions and just want to obtain the data from the module.

```c
// Helper functions for write_scratchpad 
void set_TH(uint8_t TH);
void set_TL(uint8_t TL);
void set_config(uint8_t config);
float get_temp(void);
```

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


