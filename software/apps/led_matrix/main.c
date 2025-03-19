// LED Matrix app
//
// Display messages on the LED matrix

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "led_matrix.h"
#include "microbit_v2.h"

int main(void)
{
  printf("Board started!\n");

  // initialize LED matrix driver

  // call other functions here
  led_matrix_init();

  // Test display_string_message function
  display_string_message("Hi CE346!");
  nrf_delay_ms(10000);

  display_string_message("It works!");

  // loop forever
  while (1)
  {
    nrf_delay_ms(1000);
  }
}