# Bit-Banged UART For ESP32 Microcontrollers

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

## Setting the GPIO pins

To set the GPIO pins you wish to use simply replace _desired pin_ with the pin number. This can be found in the UART_Header.h file lines 23 and 25

#define TX _desired pin_
#define GPIO_TX_PIN_SEL (1ULL << TX)
#define RX _desired pin_
#define GPIO_RX_PIN_SEL (1ULL << RX)

## How To Configure UART

UART is configured depending on the configuration functions arguments. Users can set the **baud rate**, **data frame size**, and **polarity**. Values are limited to the predefined values set in the following enumeratiions...

### Baud Rate Values
```c
typedef enum {
    Baud_Rate_0 = 9600,
    Baud_Rate_1 = 19200,
    Baud_Rate_2 = 38400,
    Baud_Rate_3 = 62500,
    Baud_Rate_4 = 125000
} baud_rate_t;
```

### Data Frame Size Values
```c
typedef enum {
    frame_size_0 = 5,
    frame_size_1 = 6,
    frame_size_2 = 7,
    frame_size_3 = 8,
    frame_size_4 = 9
} data_frame_size_t;
```

### Polarity Bit Values
```c
typedef enum {
    ON = 1,
    OFF = 0
} parity_bit_t;
```

Once you've selected the values based on your needs you simply call configure_uart with the desired values...
 
```c
configure_uart(Baud_Rate_4, frame_size_3, ON);
```

With these values the configure_uart function can then initialize the uart_config_t struct which determines the behavior of the send and receive functions. Below the uart_config struct is shown along with a declaration of a variable of type uart_config and the defintion of the config function. 

```c
typedef struct {
    baud_rate_t baud_rate;
    data_frame_size_t data_frame_size;
    parity_bit_t parity_bit;
} uart_config_t;

.
.
.

static uart_config_t uc;

.
.
.

void configure_uart(uint32_t br, uint8_t dfs, uint8_t pb){
    uc.baud_rate = br;
    uc.data_frame_size = dfs;
    uc.parity_bit = pb;
    /* DELAY needs a cheap way to divide -- 1000000 / baud_rate*/
    /*SET DELAY HERE IN MICROSECONDS*/

    switch(br){
        case 9600:
            DELAY = BR_0;
            break;
        case 19200:
            DELAY = BR_1;
            break;
        case 38400:
            DELAY = BR_2;
            break;
        case 62500:
            DELAY = BR_3;
            break;
        case 125000:
            DELAY = BR_4;
            break;
        default:
            break;
    }
    
}
```

## Data Transmission and Data Reception

UART transmissions work by sending a start bit to alert the receiver that data is going to be transmitted, sends the data bit by bit and then terminates the transmission with a stop bit. Each bit's state corresponds to either a high or low on the line. The baud rate determines how long the line is held high and low. 
```c
void uart_transmit(uint8_t * data, uint32_t size){
    ESP_ERROR_CHECK(gptimer_start(gptimer));
 
    while (size){
        byte = *(data + index);
        transmit_packet();
        size--;
        index++;
    }
    
    index = 0;
    
    gptimer_stop(gptimer);
    time_to_send_receive_data = 0;
}

static void transmit_packet(void){
    send_start_bit();
    send_data_frame();
    send_stop_bits();
}
```

UART receives by constantly monitoring for the start bit on the TX line, then reads the state of the TX line periodically depending on the baud rate and then terminates once the stop bit is received. The UART must be aware of the size of the data frame before communcation so it knows when to read for the stop bit. This means that both the transmitter and receiver must have the same data frame size in order to function correctly and reliably!

```c

void uart_receive(uint8_t * data, uint32_t size){
    while (size){
        receive_packet();
        *(data + index) = byte;         
        index++;
        size--;
        byte = 0;
    }

    index = 0;
}

static void receive_packet(void){
    receive_start_bit();
    receive_data_frame();
    receive_stop_bits();
}

```

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


























