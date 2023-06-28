
#include <defs.h>

#include "delay.h"

void delay(const uint32_t cycles) {
    /* Configure timer for a single-shot countdown */
	reg_timer0_config = 0;
	reg_timer0_data = d;
    reg_timer0_config = 1;

    // Loop, waiting for value to reach zero
    reg_timer0_update = 1;  // latch current value
    while (reg_timer0_value > 0) {
           reg_timer0_update = 1;
    }
}