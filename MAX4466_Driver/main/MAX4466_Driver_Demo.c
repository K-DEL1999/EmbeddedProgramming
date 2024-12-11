#include "MAX4466_Driver_Header.h"

adc_oneshot_unit_handle_t adc_handle;
adc_cali_handle_t adc_cali_handle;
esp_timer_handle_t periodic_timer;
//dac_oneshot_handle_t dac_handle;
//dac_continuous_handle_t dac_handle;
//i2s_chan_handle_t tx_handle;

static void init_gpio(void);
static void init_timer(void);
static void init_adc(void);
//static void init_dac(void);
//static void init_i2s(void);

void app_main(void){
    init_gpio();
    init_adc();
    init_timer();
    //init_dac();
    //init_i2s();

    usleep(1000000);
    sample_from_signal();

    //send_samples_as_signal();
    
    while (1){
    }
}

static void init_adc(void){
    // -------------------- ADC Init --------------------- //
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    adc_oneshot_new_unit(&init_config, &adc_handle);

    // -------------------- ADC Config ------------------- //
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config);

    // -------------------- ADC Calibration ------------------ // 
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    }; 

    adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
}

static void periodic_timer_callback(void* arg){
    //esp_timer_stop(periodic_timer);
    time_to_sample = 1;
    //esp_timer_start_periodic(periodic_timer, SAMPLE_RATE);
}

static void init_timer(void){
    // Initialize timer struct
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback
    };

    // Assign handler to timer struct
    esp_timer_create(&periodic_timer_args, &periodic_timer);
}


static void init_gpio(void){
    gpio_config_t io_conf = {};

    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_THRESH_HOLD_PIN_SEL;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;


    gpio_config(&io_conf);
}

/*

static void init_dac(void){
    // ------------------- DAC Init --------------------- //
    dac_oneshot_config_t init_config = {
        .chan_id = DAC_CHANNEL,
    };    

    dac_oneshot_new_channel(&init_config, &dac_handle);
    // -------------------------------------------------- //

    // ------------------ DAC_continous Init ---------------- //
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 4,
        .buf_size = 4096,
        .freq_hz = 2000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };

    dac_continuous_new_channels(&cont_cfg, &dac_handle);
    // ------------------------------------------------------ 

}

static void init_i2s(void){
    
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, &tx_handle, NULL); 

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
                    
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = BCLK_PIN,
            .ws = WS_PIN,
            .dout = DOUT_PIN,             
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    i2s_channel_init_std_mode(tx_handle, &std_cfg);

}

*/

