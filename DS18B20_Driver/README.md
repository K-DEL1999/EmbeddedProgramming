# DS18B20 Driver For ESP32 Microcontrollers

This is a driver for the DS18B20 temperature sensor written in C. It enables communcation between an ESP32 microcontroller and multiple DS18B20 temperature sensors. The means of communication is through a 1 wire protocol unique to this sensor.

## Project Structure

- **DS18B20_Source.c**: function defintions, helper function declarations, helper function defintions 
- **DS18B20_Header.h**: function declarations and struct definitions

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
void set_config(uint8_t config);
float get_temp(void);
```

## Setting the GPIO pin

You can set the pin in the DS18B20_Drvier_Header.h. You can replace <pin_number> with the desired pin

```c
#define DQ <PIN_NUMBER> // Data Line
#define GPIO_DQ_PIN_SEL (1ULL << DQ) 
```

## Example

```c
#include "DS18B20"

void app_main(){
  float var;
  set_config(2); // Value can be either 1, 2, 3 or 4

  while (1){
    var = get_temp();
    printf("Temp: %6.4f\n", var); // minimun width of 6 and 4 decimal places
  }
}

```

## How to Use

1. Ensure you have a working ESP32 development environment (this project was designed using the ESP-IDF development framework)
2. Clone the repository
3. Change the `DQ` pin numbers in the `DS18B20_Driver_Header.h` file to the pin you want to use
4. Connect devices - DQ to DQ
5. Build and Flash

### If using ESP-IDF
```sh
idf.py build
idf.py flash
```

## Application

- **Can be use this driver to read from multiple DS18B20 temperature sensors**


