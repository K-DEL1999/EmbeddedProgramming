# Bit-Banged UART For ESP32 Microcontrollers

This project is intended for educational purposes but can serve as an alterantive to the native SPI peripheral most microcontrollers house. SPI (Serial Peripheral Interface) is a synchronous protocol, meaning a clock is shared amongst all connected devices, that requires 4 lines. Of course there are variations of SPI which may require more or less than 4 lines but the main concept remains unchanged - send on rising edge, receive on falling edge 
