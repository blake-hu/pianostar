// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"

void TIMER4_IRQHandler(void);

void set_CC(void)
{

  while (list_get_first()->timer_value <= read_timer() && list_get_first() != NULL)
  {
    TIMER4_IRQHandler();
  }

  if (list_get_first() != NULL)
  {
    NRF_TIMER4->CC[2] = list_get_first()->timer_value;
  }
}

// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void)
{
  // You will place your interrupt handler code here
  if (list_get_first() == NULL || list_get_first()->timer_value > read_timer())
  {
    return;
  }

  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[2] = 0;

  printf("goh!");
  node_t *node_pointer = list_remove_first();
  node_pointer->cb();

  if (node_pointer->repeated == true)
  {
    node_pointer->timer_value = read_timer() + node_pointer->delay;
    list_insert_sorted(node_pointer);
    // NRF_TIMER4->CC[2] = list_get_first()->timer_value;
    set_CC();
  }
  else
  {
    free(node_pointer);
  }
}

// Read the current value of the timer counter
uint32_t read_timer(void)
{

  // Should return the value of the internal counter for TIMER4
  NRF_TIMER4->TASKS_CAPTURE[1] = 1;
  return NRF_TIMER4->CC[1];
}

// Initialize the timers
void virtual_timer_init(void)
{
  // Place your timer initialization code here
  NRF_TIMER4->BITMODE = 3;
  NRF_TIMER4->PRESCALER = 4;
  NRF_TIMER4->TASKS_CLEAR = 1;
  NRF_TIMER4->TASKS_START = 1;

  NRF_TIMER4->INTENSET = 0x1 << 18;
  NVIC_EnableIRQ(TIMER4_IRQn);
}

// This is a private helper function called from multiple public functions with different arguments.
// Starts a timer. This function is called for both one-shot and repeated timers
static uint32_t timer_start(uint32_t microseconds, virtual_timer_callback_t cb, bool repeated)
{
  __disable_irq();

  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
  uint32_t new_time = microseconds + read_timer();

  node_t *node_pointer = (node_t *)malloc(sizeof(node_t));
  node_pointer->timer_value = new_time;
  node_pointer->cb = cb;
  node_pointer->delay = microseconds;
  node_pointer->repeated = repeated;
  list_insert_sorted(node_pointer);

  set_CC();

  __enable_irq();

  return (uint32_t)node_pointer;
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start(uint32_t microseconds, virtual_timer_callback_t cb)
{
  return timer_start(microseconds, cb, false);
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start_repeated(uint32_t microseconds, virtual_timer_callback_t cb)
{
  return timer_start(microseconds, cb, true);
}

// Remove a timer by ID.
// Make sure you don't cause linked list consistency issues!
// Do not forget to free removed timers.
void virtual_timer_cancel(uint32_t timer_id)
{
  __disable_irq();

  node_t *node_id = (node_t *)timer_id;
  list_remove(node_id);
  free(node_id);

  set_CC();

  __enable_irq();
}
