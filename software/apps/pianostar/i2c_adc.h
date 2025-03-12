#include <stdbool.h>
#include <stdint.h>

#include "microbit_v2.h"
#include "nrf_twi_mngr.h"

#define NUM_ADC 4
#define NUM_ADC_CHANNELS 8

void i2c_init();

uint8_t i2c_adc_read(uint8_t adc_idx, uint8_t adc_channel_idx);
