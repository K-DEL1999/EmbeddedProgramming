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
### SPI Poling for Slave

You must pole (or set an interrupt) the CS line. Once the CS line is HIGH you then check the MISO line to determine whether data should be transmitted or received

```c
     while (1){
        while (gpio_get_level(SPI_CS) == 1){}       
 
        if (gpio_get_level(SPI_MOSI)){ // Master is requesting data
            spi_slave_transmit(data_tx, ARRAY_SIZE);
            if (SPIERRNO != 0){
                goto RESTART_FROM_TRANSMIT;
            }
        }
        else { // Master is sending data
            spi_slave_receive(data_rx);
            if (SPIERRNO != 0){
                goto RESTART_FROM_RECEIVE;
            }
            
            verify_data();
        }

        init_slave_spi();       
    }

```

## Setting the GPIO pins

To set the GPIO pin replace <PIN_NUMBER> with the desired pin number. Setting the pins for Master and Slave are the same. 

| Line | Function |
| :---: | :---: |
| MOSI| data out |  
| MISO | data in |
| CS | chip select |
| SCK | clock |   

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

This initialization sets up the proper conditions for an idle SPI meaning that these are the states of the pins when the microcontroller is neither transmitting nor receiving. This SPI is fixed to a **polarity of 0 and phase of 0**.  

```c  
void init_master_spi(void){
    gpio_set_level(SPI_SCK, CLOCK);
    gpio_set_level(SPI_CS, 1);
    gpio_set_level(SPI_MOSI, 0);
}
```

## Data Transmission and Data Reception

### SPI Master Transmission 

The master begins by selecting the chip and setting MOSI low to indicate the start of data transmission. The MOSI line is then updated to reflect the current bit of the byte being sent followed by a change in state of the clock. After a period of time the clock is changed again and then next bit is transmitted until all the data has been sent.

```c
void spi_master_transmit(uint8_t * data, uint32_t size){
    uint32_t index = 0;

    gpio_set_level(SPI_CS, 0); // 0 indicates that someone is occupy/controlling the bus
    gpio_set_level(SPI_MOSI, 0); // Slave reads MOSI and if low, master will transmit data
    
    // Wait for MISO confirmation --- TEST -------------------
    // Slave will indicate that it is ready by repsonding with the state of the MOSI
    while (gpio_get_level(SPI_MISO) != 0){usleep(1);} 
    // ------------------------------------------------------    

    while (size){
        for (uint32_t cnt = 0; cnt < 8; cnt++){
            gpio_set_level(SPI_MOSI, (*(data + index) & (1 << cnt)) );

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10);

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10); 
        }
      
        index++;  
        size--;
    }

    // Reset clock variables 
    clock = CLOCK;

}
```

### SPI Master Reception

To receive data, the master selects the chip and indicates it is ready to receive by setting the MOSI line high. The data is sent bit by bit and follows the same clock updating mechanism as before but instead of setting the MOSI line it reads the MISO line.

```c
void spi_master_receive(uint8_t * data){ // returns number of bytes of data
    uint32_t byte = 0, index = 0, size = ARRAY_SIZE;

    gpio_set_level(SPI_CS, 0); // 0 indicates that someone is occupy/controlling the bus
    gpio_set_level(SPI_MOSI, 1); // slave reads MOSI and if high, master is requesting data   

    // Wait for MISO confirmation --- TEST -------------------
    // Slave will indicate that it is ready by returning the state of the MOSI
    while (gpio_get_level(SPI_MISO) != 1){usleep(1);} 
    // ------------------------------------------------------

    while(size){
        byte = 0;
        
        for (uint32_t cnt = 0; cnt < 8; cnt++){

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10); 

            byte |= (gpio_get_level(SPI_MISO) << cnt);

            clock = !clock;
            gpio_set_level(SPI_SCK, clock);

            usleep(10); 
        }

        *(data + index) = byte;
        index++;
        size--;
    }
    
    clock = CLOCK;

}
```

### SPI Slave Reception

The slave receive function runs while the CS line is set HIGH. It keeps track of the clock and only reads from the line on a rising edge. Using bit manipulation it reconstructs the data bit by and bit allow for a reliable and efficient serial transfer of data. The function is dependent on both the state of the SCK pin and the CS pin. The slave can only receive when the CS pin is set HIGH and the MOSI is set HIGH.

```c
void spi_slave_receive(uint8_t * data){
    uint8_t byte = 0, cnt = 0, index = 0; 
    clockWatchDog = 0;

    gpio_set_level(SPI_MISO, 0);

    while (!gpio_get_level(SPI_CS)){
        if (clock == CLOCK && gpio_get_level(SPI_SCK) == !CLOCK){
            usleep(5);
            byte |= (gpio_get_level(SPI_MOSI) << cnt);
            cnt++;
            if (cnt == 8){
                *(data + index) = byte;
                index++;
                cnt = 0;
                byte = 0;
            }
            clock = !CLOCK;
            clockWatchDog = 0;
        }

        if (gpio_get_level(SPI_SCK) == CLOCK) { 
            clock = CLOCK; 
        }
        
        clockWatchDog++;
        if (clockWatchDog > 100000){
            SPIERRNO |= 1;
            return;
        }
    }
}
```

### SPI Slave Transmission

The slave transmission function is very similar to the slave reception function but instead of reading from the MOSI it writes to the MISO. The slave cannot initiate a data transfer but has to be signaled by the master to send data. When selecting the chip (setting CS HIGH on one of the CS lines) the master also sets the MOSI to HIGH or LOW depending on whether it wants to send or receive from said chip. If MOSI is HIGH when setting CS then master is sending and if MOSI is LOW than master is requesting data.

```c
void spi_slave_transmit(uint8_t * data, uint32_t size){
    uint8_t cnt = 0, index = 0;
    clockWatchDog = 0;

    gpio_set_level(SPI_MISO, 1);

    while (!gpio_get_level(SPI_CS)){
        if (clock == CLOCK && gpio_get_level(SPI_SCK) == !CLOCK){
            gpio_set_level(SPI_MISO, ((((*(data + index)) >> cnt) & 1)));
            cnt++;
            if (cnt == 8){
                cnt = 0;
                index++;
            }
            clock = !CLOCK;
            clockWatchDog = 0;
        }

        if (gpio_get_level(SPI_SCK) == CLOCK) { 
            clock = CLOCK; 
        }
        
        clockWatchDog++;
        if (clockWatchDog > 100000){
            SPIERRNO |= (1 << 1);
            return;
        }

    }
}
```

## How to Use

1. Ensure you have a working ESP32 development environment (this project was designed using the ESP-IDF development framework)
2. Clone the repository
3. Flash transmitter with Master SPI driver
4. Flash receiver with Slave SPI driver
5. Change the `MOSI`, `MISO`, `SCK` and `CS` pin numbers in the `SPI__Header.h` file to pins you wish to use
6. Connect devices - connect MOSI pin in one device to the MISO pin in the other 
7. Connect devices - connect MISO pin in one device to the MOSI pin in the other
8. Connect devices - connect SCK pin in one device to the SCK pin in the other
9. Connect devices - connect CS pin in one device to the CS pin in the other

### If using ESP-IDF
```sh
idf.py build
idf.py flash
```

## Application

- **Can be used as an alternative to the built in ESP32 SPI peripheral**
- **Better understand the inner workings of the SPI protocol (Polarity: 0, Phase: 0)** 



