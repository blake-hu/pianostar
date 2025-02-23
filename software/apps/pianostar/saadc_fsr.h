#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

// Analog input pin
#define ANALOG_FSR_IN_0 NRF_SAADC_INPUT_AIN0
#define ANALOG_FSR_IN_1 NRF_SAADC_INPUT_AIN1
#define ANALOG_FSR_IN_2 NRF_SAADC_INPUT_AIN2

// ADC channel configuration
#define ADC_FSR_CHANNEL_0 0
#define ADC_FSR_CHANNEL_1 1
#define ADC_FSR_CHANNEL_2 2
#define ADC_FSR_CHANNEL_FIRST ADC_FSR_CHANNEL_0
#define ADC_FSR_CHANNEL_LAST ADC_FSR_CHANNEL_2

void adc_init(void);
float adc_sample_blocking(uint8_t channel);