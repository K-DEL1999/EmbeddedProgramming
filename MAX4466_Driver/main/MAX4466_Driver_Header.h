#ifndef MAX4466_DRIVER_HEADER_H
#define MAX4466_DRIVER_HEADER_H

#include <unistd.h>
#include "esp_adc/adc_oneshot.h"
//#include "driver/dac_oneshot.h"
//#include "driver/dac_continuous.h"
#include <esp_timer.h>
//#include "driver/i2s_std.h"
#include "driver/gpio.h"

#define HIGH 1
#define LOW 0


// ---------- LEDs PINs --------- //
#define THRESH_HOLD_1 19
#define THRESH_HOLD_2 18
#define THRESH_HOLD_3 5 
#define THRESH_HOLD_4 17
#define GPIO_THRESH_HOLD_PIN_SEL ((1ULL << THRESH_HOLD_1) | (1ULL << THRESH_HOLD_2) | (1ULL << THRESH_HOLD_3) | (1ULL << THRESH_HOLD_4)) 
// ----------------------------- //


// -------- ADC and DAC channels ------------- //
#define ADC_CHANNEL ADC_CHANNEL_3
//#define DAC_CHANNEL DAC_CHAN_0
// ------------------------------------------- //

/*
// ---------- I2S pins --------------- //
#define WS_PIN 5 
#define BCLK_PIN 16
#define DOUT_PIN 18
#define GPIO_I2S_PIN_SEL ((1ULL << WS_PIN) | (1ULL << BCLK_PIN) | (1ULL << DOUT_PIN))
// ----------------------------------- //
*/


//#define NUM_OF_SAMPLES 44100
#define SAMPLE_RATE 100000


// ----- Source Externs ----------------- //
// Flag for sampling
extern bool time_to_sample;
//-------------------------------------- //

// ----- Main Externs ------------------- //
// Timer from main
extern esp_timer_handle_t periodic_timer;
// ADC handler
extern adc_oneshot_unit_handle_t adc_handle;
// ADC calibration handler
extern adc_cali_handle_t adc_cali_handle;
// DAC handler
//extern dac_oneshot_handle_t dac_handle;
// I2S tx_handler
//extern i2s_chan_handle_t tx_handle;
// -------------------------------------- //

void sample_from_signal(void);
//void send_samples_as_signal(void);

#endif /* MAX4466_DRIVER_HEADER_H */
