#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Initialize the LED matrix display
void led_matrix_init();

// set all leds to ON
void set_leds();

// start/stop transfer
void start_transfer();
void stop_transfer();
