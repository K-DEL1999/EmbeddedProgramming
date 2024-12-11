#include <MAX4466_Driver_Header.h>

#define DC_OFFSET 2047 // half of 4095

bool time_to_sample;
int sample;
//uint16_t processed_samples[NUM_OF_SAMPLES];

void sample_from_signal(void){
    esp_timer_start_periodic(periodic_timer, SAMPLE_RATE);
   
    //uint32_t i = 1; 
    //while (i < NUM_OF_SAMPLES){
    while (1){
        adc_oneshot_read(adc_handle, ADC_CHANNEL, &sample);
        //adc_cali_raw_to_voltage(adc_cali_handle, samples[i], &voltages[i]);
        //processed_samples[i] = (sample < 0) ? 0 : (sample & 0xFFFF);
        //processed_samples[i] = processed_samples[i-1] + ( (i % 2) ? 0 : 1 );
        sample = abs(sample - DC_OFFSET);
        gpio_set_level(THRESH_HOLD_1, ((sample > 400) ? HIGH : LOW)); 
        gpio_set_level(THRESH_HOLD_2, ((sample > 600) ? HIGH : LOW)); 
        gpio_set_level(THRESH_HOLD_3, ((sample > 800) ? HIGH : LOW)); 
        gpio_set_level(THRESH_HOLD_4, ((sample > 1000) ? HIGH : LOW)); 
        //i++;
        
        while (!time_to_sample){}
        time_to_sample = 0;
    }
    
    esp_timer_stop(periodic_timer);
    
    /*for (int i = 0; i < NUM_OF_SAMPLES; i++){
        printf("%d ", processed_samples[i]);
    }
    printf("\n");*/
}

/*
void send_samples_as_signal(void){
    size_t bytes = 2*NUM_OF_SAMPLES;
    while (bytes == 2*NUM_OF_SAMPLES){
        i2s_channel_preload_data(tx_handle, processed_samples, NUM_OF_SAMPLES, &bytes);
    }

    i2s_channel_enable(tx_handle);
    
    //while (1){
        i2s_channel_write(tx_handle, processed_samples, NUM_OF_SAMPLES, &bytes, 1000); 
    //}
    
    i2s_channel_disable(tx_handle);

    
    for (int i = 0; i < NUM_OF_SAMPLES; i++){
        printf("%d ", processed_samples[i]);
    }
    printf("\n");
    
}*/
