# Bit-Banged I2C For ESP32 Microcontrollers

This project was my gate way into the world of embedded systems and introduced me to the idea of bit banging. I2C (Inter-Intergrated Circuit) is a synchronous 2 wire - SDA and SCL - serial communcation protocol. It is among the most popular serial communication protocols and is very efficient in short distance communcation with mutliple devices/peripherals. Microcontrollers will typically have an onboard I2C peripheral but this driver offers an alternative that shows users how the protocol works. The driver is does not challenge the native I2C protocol in any performance aspects ( since it requires CPU time to perform the data transfer while the peripheral doesnt ) but it does teach users of the driver how I2C can be implemented. With bit banging GPIO pins are manually set HIGH and LOW to reflect the states of the clock and the bits in the data. Therefore 2 GPIO pins need to be intialized to perform any kind of bit banging. This driver can be used as an alternative to the built in I2C peripheral but if speed and reliablity are what you seek than defer to the onboard I2C. 

## Project Structure

### Master Driver
- **I2C_Master_Source.c**: function defintions, helper function declarations, helper function defintions 
- **I2C_Master_Header.h**: function declarations and struct definitions
- **I2C_Master_Demo.c**: example showing how to use I2C Master driver functions

### Slaver Driver
- **I2C_Slave_Source.c**: function defintions, helper function declarations, helper function definitions 
- **I2C_Slave_Header.h**: function declarations and struct defiintions
- **I2C_Slave_Demo.c**: example showing how to use I2C Slave driver functions

## Available I2C Master Functions 
```c
uint32_t I2C_transmit(uint8_t * data, uint32_t size);
uint32_t I2C_receive(uint8_t * data, uint32_t size);
```
Users are provided only a transmit and receive function. No initialization function is needed due to the simplicity of the I2C protocol

## Available I2C Slave Functions 
```c
uint32_t I2C_await_request(uint8_t * data, uint32_t size);
```
Users are provided an await function which poles the SDA line until an address is transmitted. If the address does not match the slave devices address then it returns to poling but if it does match then the device determines whether it will send or receive data depending on the MSB of the address sent - 0 : receive from master, 1 : transmit to master 

## Setting the GPIO pins

For both the Master and Slave drivers you set the pins by updating the values in the Master/Slave Header file. Replace <PIN_NUMBER> with the desired pins you wish to assign that SDA and SCL

```c
#define I2C_SDA <PIN_NUMBER>
#define GPIO_SDA_PIN_SEL (1ULL << I2C_SDA)

#define I2C_SCL <PIN_NUMBER>
#define GPIO_SCL_PIN_SEL (1ULL << I2C_SCL)
```

## How To Configure I2C

There is not much configuration needed for the I2C protocol due to its simplicity. The data frame size is strcitly 8 bits (1 byte) but you could change the address. However you must have 2 copies of the address - one with MSB of 0 and another with MSB of 1. One tells the slave its receiving while the other tells the slave it is transmitting. 

You can update the address for the master in the I2C_Master_Source.c
```c
#define SLAVE_ADDRESS_WRITE 0b01111001
#define SLAVE_ADDRESS_READ 0b11111001
```

You can update the addresses for the slave in the I2C_Slave_Header.h
```c
#define SLAVE_ADDRESS_WRITE 0b01111001
#define SLAVE_ADDRESS_READ 0b11111001
```
  
**If changing addresses remember they MUST BE THE SAME !**

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

