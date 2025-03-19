#pragma once

#include <stdbool.h>

// Starts continuously measuring capacitive touch for the logo
// Function returns immediately without blocking
void capacitive_touch_init(void (*on_touch)(void));

// disable interrupts from timer and capacitive touch sensor
void disable_interrupts(void);
