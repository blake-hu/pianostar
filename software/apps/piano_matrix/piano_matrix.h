#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"

// initializers
void led_matrix_init(void);
void timer_init(void);

// for debugging
uint32_t read_timer(void);

// display 256 LEDs
// buffer: 256 32-bit integers
// only using bottom 24 bits
void display_buffer(void);