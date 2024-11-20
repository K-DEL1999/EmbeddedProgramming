#ifndef DS18B20_DRIVER_HEADER_H
#define DS18B20_DRIVER_HEADER_H

#include <unistd.h>
#include <driver/gpio.h>

#define HIGH 1
#define LOW 0

#define DQ 16 // Data Line
#define GPIO_DQ_PIN_SEL (1ULL << DQ)

#define NUM_OF_DEVICES 1
// --- From source file ----------------------------//
extern uint64_t device_ROM_numbers[NUM_OF_DEVICES];
extern uint8_t devices_found;
extern uint8_t active_alarms_found;
// ------------------------------------------------//

// ------------------------------------------------//
// Public Functions
// ------------------------------------------------//
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
// ------------------------------------------------//
// ------------------------------------------------//

// Helper functions for write_scratchpad 
void set_TH(uint8_t TH);
void set_TL(uint8_t TL);
void set_config(uint8_t config);
float get_temp(void);

#endif /*DS18B20_DRIVER_HEADER_H*/


