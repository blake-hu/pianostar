// Breadboard example app
//
// Read in from force-sensitive resistor

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

// Analog input pin
#define ANALOG_FSR_IN NRF_SAADC_INPUT_AIN1

// ADC channel configuration
#define ADC_FSR_CHANNEL 0

// Function prototypes
static void adc_init(void);
static float adc_sample_blocking(uint8_t channel);

static void saadc_event_callback(nrfx_saadc_evt_t const *_unused)
{
  // don't care about saadc events
  // ignore this function
}

static void adc_init(void)
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

  printf("pizza\n");

  // Initialize FSR channel
  nrf_saadc_channel_config_t fsr_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_FSR_IN);
  printf("banana\n");
  error_code = nrfx_saadc_channel_init(ADC_FSR_CHANNEL, &fsr_channel_config);
  printf("saadc channel error code: %ld\n", error_code);
  APP_ERROR_CHECK(error_code);
}

static float adc_sample_blocking(uint8_t channel)
{
  // read ADC counts (0-4095)
  // this function blocks until the sample is ready
  int16_t adc_counts = 0;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &adc_counts);
  APP_ERROR_CHECK(error_code);

  printf("adc counts: %d\n", adc_counts);

  // convert ADC counts to volts
  float voltage = ((float)adc_counts / 4095) * 3.6;

  return voltage;
}

int main(void)
{
  printf("Board started!\n");

  // init ADC
  adc_init();

  while (1)
  {
    // sample FSR
    float fsr_voltage = adc_sample_blocking(ADC_FSR_CHANNEL);
    printf("FSR voltage: %f\n", fsr_voltage);

    // wait a little bit of time
    nrf_delay_ms(1000);
  }
}