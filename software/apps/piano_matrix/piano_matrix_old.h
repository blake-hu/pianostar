#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"

// Initialize the LED matrix display
void led_matrix_init(void);

// init timers
void timer_init(void);

// for debugging
uint32_t read_timer_3(void);
uint32_t read_timer_4(void);

// display 256 LEDs
// buffer: 256 32-bit integers
// only using bottom 24 bits
void display_buffer(uint32_t *buf);