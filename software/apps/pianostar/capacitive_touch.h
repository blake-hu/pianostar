#pragma once

#include <stdbool.h>

// Starts continuously measuring capacitive touch for the logo
// Function returns immediately without blocking
void capacitive_touch_init(void (*touch_callback)(void));
