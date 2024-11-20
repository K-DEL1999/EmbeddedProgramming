#include <DS18B20_Driver_Header.h>

#define RECOVERY_TIME_SLOT() (usleep(1))

// Variables
static volatile uint32_t cnt;

// Array of addresses and devices count
uint64_t device_ROM_numbers[NUM_OF_DEVICES];
uint8_t devices_found;
uint8_t active_alarms_found;

#define BYTES 9
#define BITS 8
static bool scratchpad_memory[BYTES][BITS];

// Configuration Register, TH and TL registers
uint8_t config_byte = 0b00011111;
uint8_t config_val;
uint8_t TH_byte = 0b01111110;
uint8_t TL_byte = 0b11001001;

// Variables for One Wire Search ROM algorithm
//--------------------------------------------------------------------------------------//
static uint64_t ROM_NO; // actual ROM number being constructed
static uint8_t id_bit_number; // current position in ROM number
static bool id_bit; // first read from devices -- AND of all bit values
static bool comp_id_bit; // second read from devices -- complemented AND of all bit values
static uint8_t last_zero; // position were we last wrote a 0 due to a discrepency
static bool search_direction; // direction of search -- which branch/path to take

static bool last_device_flag; // flag to indicate previous searh was the last device
static uint8_t last_discrepency; // position where the last discprepency was detected
//--------------------------------------------------------------------------------------//

// -------------------------------------//
// Private Functions
// -------------------------------------//
static bool initialize(void);
static void send_cmd(uint8_t cmd);
static void write_0_time_slot(void);
static void write_1_time_slot(void);
static bool read_time_slot(void);

// One write search algortihm function
static bool find_ROM_numbers(void);
static bool one_wire_search_algorithm(void);
static bool one_wire_verify(void);

// CRC generater and check
static bool shift_register[8];
static bool crc_verification(uint64_t ROM);

// -------------------------------------//
// -------------------------------------//
// -------------------------------------//


// Get temperature
float get_temp(void){
    match_rom(device_ROM_numbers[0]);
    read_scratchpad();
    
    float temp_calculated = 0;
    
    for (int i = 0; i < 3; i++){
        temp_calculated += (scratchpad_memory[1][i] << (i+4));
    }
    
    for (int i = 4; i < 8; i++){
        temp_calculated += (scratchpad_memory[0][i] << (i-4));
    }

    int power =  2;
    for (int i = 0; i < config_val; i++){
        temp_calculated += (scratchpad_memory[0][i]) ? (1.0/power) : 0;
        power *= 2;
    }

    return temp_calculated * ((scratchpad_memory[1][3]) ? -1 : 1); 
}

static bool initialize(void){
    gpio_set_level(DQ, LOW); // Send reset pulse=
    usleep(480);
    gpio_set_level(DQ, HIGH); // Release bus
    usleep(120);
    
    if (gpio_get_level(DQ)){
        usleep(360);
        return 1; // Module did not respond    
    } 
    
    usleep(360);
    return 0; // Module is awake and awaiting cmd
}

// --------------------------------------------------- //
// ---------- ROM function commands ------------------ //
// --------------------------------------------------- //

// ------------------------------------------------------------//
// Only for use when there is a single DS18B20 on the bus
// Single drop bus system is a system with only one connected device
//  hence the  "single drop" -- multi-drop bus system is a 
//  system with multiple devices connected to the bus.
// ------------------------------------------------------------//
bool skip_rom(void){
    if (initialize()){
        return 1;
    }

    send_cmd(0xCC);

    return 0;
}

bool read_rom(void){
    if (initialize()){
        return 1;
    }
    
    send_cmd(0x33);

    for (; cnt < 64; cnt++){
        id_bit = read_time_slot();
        ROM_NO |= (id_bit << cnt);
    }
    cnt = 0;

    return 0;
}
// ------------------------------------------------------------//
// ------------------------------------------------------------//
// ------------------------------------------------------------//

bool match_rom(uint64_t rom_no){
    if (initialize()){
        return 1;
    }

    send_cmd(0x55);

    for (; cnt < 64; cnt++){
        ((rom_no >> cnt) & 1) ? write_1_time_slot() : write_0_time_slot();
    }
    cnt = 0;

    return 0;
}

/*
Search ROM uses a 1 wire search algorithm which is 
valid for all current and future devices that 
feature a 1-wire interface.

This is used to discover the 64-bit unique registration
number of all devices connected to the 1 wire bus
*/

bool search_rom(void){
    if (initialize()){
        return 1;
    }
    
    // Sending the "search ROM" cmd readies the 1 wire 
    // devices to begin the search
    send_cmd(0xF0);
    
    return find_ROM_numbers();
}

bool alarm_search(void){
    if (initialize()){
        return 1;
    }
    
    // Sending the "alarm search ROM" cmd readies the 1 wire 
    // devices to begin the search. Only DS18B20s which have
    // enountered an alarm condition will respond to this cmd.
    send_cmd(0xEC);
    
    uint8_t backup_devices_found = devices_found;

    if (find_ROM_numbers()){
        active_alarms_found = devices_found;
        devices_found = backup_devices_found;
        return 1;
    }
    else {
        active_alarms_found = devices_found;
        devices_found = backup_devices_found;
        return 0;
    }
}

// --------------------------------------------------- //
// --------------------------------------------------- //


// --------------------------------------------------- //
// ---------- Memory function commands --------------- //
// --------------------------------------------------- //
void write_scratchpad(void){
    send_cmd(0x4E);

    for (int i = 0; i < 8; i++){
        (TH_byte & 1) ? write_1_time_slot() : write_0_time_slot();
        TH_byte >>= 1;
    }

    for (int i = 0; i < 8; i++){
        (TL_byte & 1) ? write_1_time_slot() : write_0_time_slot();
        TL_byte >>= 1;
    }

    for (int i = 0; i < 8; i++){
        (config_byte & 1) ? write_1_time_slot() : write_0_time_slot();
        config_byte >>= 1;
    }
}

void set_TH(uint8_t TH){
   TH_byte = TH; 
}

void set_TL(uint8_t TL){
    TL_byte = TL;
}

void set_config(uint8_t config){
    switch(config){
        case 1:
            config_byte = 0b00011111;
            config_val = 1;
            break;
        case 2: 
            config_byte = 0b00111111;
            config_val = 2;
            break;
        case 3:
            config_byte = 0b01011111;
            config_val = 3;
            break;
        case 4:
            config_byte = 0b01111111;
            config_val = 4;
            break;
        default:
            break;
    }
}

void read_scratchpad(void){
    send_cmd(0xBE);

    for (int byte = 0; byte < 9; byte++){
        for (int bit = 0; bit < 8; bit++){
            scratchpad_memory[byte][bit] = read_time_slot();
        }
    }    
}

void copy_scratchpad(void){
    send_cmd(0x48);

    // Waits for task to finsih 
    while (!read_time_slot()){}
}

void convert_temp(void){
    send_cmd(0x44);
    
    // Waits for task to finsih 
    while (!read_time_slot()){}
}

// Recalls temp trigger values and configuration register
//  stored in E2 to the scratchpad
// Outputs its temperature converter busy flag
// Every read_time_slot() issued after the cmd will output
//  its temp converter busy flag 0 = busy, 1 = ready
void recall_E2(void){
    send_cmd(0xB8);
   
    // Waits for task to finsih 
    while (!read_time_slot()){}
}
 
bool read_power_supply(void){
    send_cmd(0xB4);

    return read_time_slot();
}

// --------------------------------------------------- //
// --------------------------------------------------- //

// ---------------------------------------------------------- //
// Check CRC code after reading or writing from/to scratchpad //
// ---------------------------------------------------------- //
static bool crc_verification(uint64_t ROM){
    // modulo_2_polynomial_division with binary coefficient 
    // polynomial in this case is the binary coefficient of the 
    //  polynomial function of the CRC -- x^8 + x^5 + x*4 + 1.
    //  the binary coefficient of this polynomial is 00110001  

    // crc is an error checker
    // maxims CRC -- 8 bits CRC with polynomial x^8 + x^5 + x^4 + 1
    bool input;

    // polynomial we are working with is x^8 + x^5 + x^4 + 1
    // The CRC is dependent on the data preceding it. In our case
    //  the CRC is the most significant byte and can be calculated from
    //  the first 56 bits of our 64 bit ROM. The CRC is kept in the 
    //  Most Significant byte so we can check if there was an error by
    //  comparing our computed value to the most significant byte of the 
    //  ROM_NO. 
    for (int bit_position = 0; bit_position < 56; bit_position++){
        input = (ROM & 1) ^ shift_register[7];
        shift_register[7] = shift_register[6];
        shift_register[6] = shift_register[5];
        shift_register[5] = shift_register[4] ^ input;
        shift_register[4] = shift_register[3] ^ input;
        shift_register[3] = shift_register[2];
        shift_register[2] = shift_register[1];
        shift_register[1] = shift_register[0];
        shift_register[0] = input;
        ROM >>= 1;
    }    
    
    for (int i = 0; i < 8; i++){
        if ((shift_register[7-i]) != ((ROM >> (55+i)) & 1)){
            return 1;
        }
    } 

    return 0;
}

// ---------------------------------------------------------- //
// ---------------------------------------------------------- //

//------------------------------------------------------------------------------------------------------ //
// One Wire Search Algorithm functions
//------------------------------------------------------------------------------------------------------ //
// ROM numbers refer to registration number or address 
static bool find_ROM_numbers(void){
    // Everything is initiated by the master
    // So to receive data, we must send a 
    // read_time_slot().
    
    /*  The algorithm
        1. All participating devices simultaneously 
            send their first bit (lsb). Due to the 
            characteristic of 1 wire communication,
            when all devices respond at the same time,
            this results in a logical AND of the bits
            sent. 
        2. After the first bit, the devices then send 
            the complement of the first bit. These 2
            bits allow us to derive info on the first
            bit of the registration of all participating
            devices.
        3. The 1 wire master then sends the a bit back
            to all participating devices. If the device's
            have that bit value then they continue
            participating. Otherwise it remains idle until
            the next 1 wire reset.
        4. This read 2 bits, write 1 bit pattern is 
            repeated 63 more times ultimately finding
            a 64-bit registration number (also know as
            ROM number). In each iteration a different path
            will be taken until all paths have been explored.    
    
        When a 1,1 is encountered, this means the module has 
        disconnected from the bus during search so we have to
        restart our search for that iteration. This is done 
        by reseting the last_discrepency and last_device_flag.
    
    */
    // This algo indexes from 1 to 64, not 0 to 63
    // ******
    // ******
    // Discrepency refers to encountering a 0,0 from the 2 reads
    // ******
    // ******
    devices_found = 0;
    
    while (!last_device_flag){
        while (one_wire_search_algorithm()){
            last_discrepency = 0;
            last_device_flag = 0;
        }

        if (!one_wire_verify()){
            device_ROM_numbers[devices_found] = ROM_NO;
            devices_found++;  
        }
    }     

    if (device_ROM_numbers[devices_found] == 0){
        return 1;
    }

    cnt = 0;

    return 0;   
}

// Algorithm for finding ROM_NO
static bool one_wire_search_algorithm(void){
    id_bit_number = 1;
    last_zero = 0;

    //uint64_t shifted_bit;

    for (; cnt < 64; cnt++){
        id_bit = read_time_slot();
        comp_id_bit = read_time_slot();
        
        if (id_bit & comp_id_bit){
            // Log no devices detected
            // Must reset search
            return 1;
        }
        else if (id_bit != comp_id_bit){
            search_direction = id_bit;
        }
        else {
            if (id_bit_number == last_discrepency){
                search_direction = 1;
            }
            else if (id_bit_number > last_discrepency){
                search_direction = 0;
            }
            else {
                search_direction = ((ROM_NO >> (id_bit_number-1)) & 1);
            }

            if (!search_direction){
                last_zero = id_bit_number;
            }
        }

        //shifted_bit = (1ULL << (id_bit_number - 1));
        ROM_NO &= ~(1ULL << (id_bit_number - 1));

        //shifted_bit = (search_direction) ?  (1ULL << (id_bit_number - 1)) : 0;
        ROM_NO |= ((search_direction) ? (1ULL << (id_bit_number - 1)) : 0);
        
        //ROM_NO &= ~(1 << (id_bit_number - 1));
        //ROM_NO |= (search_direction << (id_bit_number - 1));
        
        (search_direction) ? write_1_time_slot() : write_0_time_slot();
   
        // Move on to next node in tree  
        id_bit_number++;

        if (id_bit_number > 64){
            last_discrepency = last_zero;
            
            if (last_discrepency == 0){
                last_device_flag = 1; 
            }
        }
    }

    return 0;
} 

// verify that ROM_NO found is valid
static bool one_wire_verify(void){
    uint8_t backup_last_discrepency = last_discrepency;
    bool backup_last_device_flag = last_device_flag; 

    last_discrepency = 64;
    last_device_flag = 0;

    bool result = one_wire_search_algorithm(); 

    last_discrepency = backup_last_discrepency;
    last_device_flag = backup_last_device_flag;

    return result;
}
//------------------------------------------------------------------------------------------------------ //
//------------------------------------------------------------------------------------------------------ //
//------------------------------------------------------------------------------------------------------ //


// Write cmds ------------------------------ //
static void write_1_time_slot(void){
    gpio_set_level(DQ, LOW);
    usleep(5);   
    gpio_set_level(DQ, HIGH);
    usleep(55);
    // Recovery time between individual time slots
    RECOVERY_TIME_SLOT(); //usleep(1); 
}

static void write_0_time_slot(void){
    gpio_set_level(DQ, LOW);
    usleep(60);
    gpio_set_level(DQ, HIGH);
    // Recovery time between individual time slots
    RECOVERY_TIME_SLOT(); //usleep(1); 
}
// ------------------------------------------- //

// Read cmd --------------------------------- //
static bool read_time_slot(void){
    gpio_set_level(DQ, LOW);
    usleep(1);
    gpio_set_level(DQ, HIGH);
    usleep(10);
    
    if (gpio_get_level(DQ)){
        usleep(50);

        // Recovery time between individual time slots
        RECOVERY_TIME_SLOT(); //usleep(1); 
        return 1;
    }
    else {
        usleep(50);

        // Recovery time between individual time slots
        RECOVERY_TIME_SLOT(); //usleep(1); 
        return 0;
    }
}

// ------------------------------------------- //

static void send_cmd(uint8_t cmd){
    while (cnt < 8){
        if (cmd & 1){
            write_1_time_slot();    
        }
        else {
            write_0_time_slot();    
        }

        cmd >>= 1;
        cnt++;
    }

    cnt = 0;
}



