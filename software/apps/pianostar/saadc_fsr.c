#include "saadc_fsr.h"

static void saadc_event_callback(nrfx_saadc_evt_t const *_unused)
{
  // don't care about saadc events
  // ignore this function
}

void adc_init(void)
{
    // Initialize the SAADC
    nrfx_saadc_config_t saadc_config = {
        .resolution = NRF_SAADC_RESOLUTION_12BIT,
        .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
        .interrupt_priority = 4,
        .low_power_mode = false,
    };
    ret_code_t error_code = nrfx_saadc_init(&saadc_config, saadc_event_callback);
    printf("saadc init error code: %ld\n", error_code);
    APP_ERROR_CHECK(error_code);

    // Initialize FSR channel
    nrf_saadc_channel_config_t fsr_channel_config_0 = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_FSR_IN_0);
    nrf_saadc_channel_config_t fsr_channel_config_1 = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_FSR_IN_1);
    nrf_saadc_channel_config_t fsr_channel_config_2 = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_FSR_IN_2);
    error_code = nrfx_saadc_channel_init(ADC_FSR_CHANNEL_0, &fsr_channel_config_0);
    APP_ERROR_CHECK(error_code);
    error_code = nrfx_saadc_channel_init(ADC_FSR_CHANNEL_1, &fsr_channel_config_1);
    APP_ERROR_CHECK(error_code);
    error_code = nrfx_saadc_channel_init(ADC_FSR_CHANNEL_2, &fsr_channel_config_2);
    APP_ERROR_CHECK(error_code);
}

float adc_sample_blocking(uint8_t channel)
{
    // read ADC counts (0-4095)
    // this function blocks until the sample is ready
    int16_t adc_counts = 0;
    ret_code_t error_code = nrfx_saadc_sample_convert(channel, &adc_counts);
    APP_ERROR_CHECK(error_code);

    // convert ADC counts to volts
    float voltage = ((float)adc_counts / 4095) * 3.6;

    return voltage;
}
