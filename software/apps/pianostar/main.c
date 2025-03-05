#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"
#include "pwm_speaker.h"
#include "notes.h"
#include "saadc_fsr.h"

const float V_TO_VOL_SCALE = 1.0 / 3.6;
const float PLAY_THRESHOLD = 1.0;

pianostar_note_t convert_fsr_to_note(uint8_t channel, float voltage) {
	float volume = 0.0;
	
	if (voltage < PLAY_THRESHOLD) {
		volume = 0;
	} else {
		volume = voltage * V_TO_VOL_SCALE;
	}
	
	uint16_t frequency = 0;
	switch (channel) {
		case ADC_FSR_CHANNEL_0:
			frequency = C4;
			break;
		case ADC_FSR_CHANNEL_1:
			frequency = E4;
			break;
		case ADC_FSR_CHANNEL_2:
			frequency = G4;
			break;
		default:
			break;
	}
	pianostar_note_t note = {
		.frequency = frequency,
		.volume = volume
	};
	return note;
}

int main(void)
{
	printf("Board started!\n");

	// init ADC
	adc_init();
	// printf("adc init\n");

	// initialize GPIO
	gpio_init();
	// printf("gpio init\n");

	// initialize the PWM
	pwm_init();
	// printf("pwm init\n");

	// compute the sine wave values
	// You should pass in COUNTERTOP-1 here as the maximum value
	compute_sine_wave((16000000 / (SAMPLING_FREQUENCY * 2)) - 1);
	// printf("computed sine wave\n");

	while (1)
	{
		// sample FSR
		float fsr_voltage_0 = adc_sample_blocking(ADC_FSR_CHANNEL_0);
		// printf("fsr 0: %f\n", fsr_voltage_0);
		pianostar_note_t note_0 = convert_fsr_to_note(ADC_FSR_CHANNEL_0, fsr_voltage_0);
		float fsr_voltage_1 = adc_sample_blocking(ADC_FSR_CHANNEL_1);
		// printf("fsr 1: %f\n", fsr_voltage_1);
		pianostar_note_t note_1 = convert_fsr_to_note(ADC_FSR_CHANNEL_1, fsr_voltage_1);
		float fsr_voltage_2 = adc_sample_blocking(ADC_FSR_CHANNEL_2);
		// printf("fsr 2: %f\n", fsr_voltage_2);
		pianostar_note_t note_2 = convert_fsr_to_note(ADC_FSR_CHANNEL_2, fsr_voltage_2);
		// printf("sampled fsr\n");

		clear_notes();
		// printf("cleared notes\n");
		add_note(note_0);
		add_note(note_1);
		add_note(note_2);
		// printf("added notes\n");
		play_updated_notes();
		// printf("play notes\n");
		nrf_delay_ms(200);
	}

	pwm_stop();
}
