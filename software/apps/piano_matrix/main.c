// LED Matrix app
//
// Display messages on the LED matrix

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "piano_matrix.h"
#include "microbit_v2.h"

int main(void)
{
  printf("Board started!\n");

  // initialize LED matrix driver
  led_matrix_init();

  // turn everything on
  set_leds();
  start_transfer();

  // loop forever
  while (1)
  {
    nrf_delay_ms(1000);
    printf("hi ");
  }
}
