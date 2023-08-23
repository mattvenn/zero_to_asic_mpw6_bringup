#include <defs.h>
//#include <math.h>
#include <stub.h>
#include <hw/common.h>
#include <uart.h>
#include <uart_api.h>

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

void configure_io()
{

//  ======= Useful GPIO mode values =============

//      GPIO_MODE_MGMT_STD_INPUT_NOPULL
//      GPIO_MODE_MGMT_STD_INPUT_PULLDOWN
//      GPIO_MODE_MGMT_STD_INPUT_PULLUP
//      GPIO_MODE_MGMT_STD_OUTPUT
//      GPIO_MODE_MGMT_STD_BIDIRECTIONAL
//      GPIO_MODE_MGMT_STD_ANALOG

//      GPIO_MODE_USER_STD_INPUT_NOPULL
//      GPIO_MODE_USER_STD_INPUT_PULLDOWN
//      GPIO_MODE_USER_STD_INPUT_PULLUP
//      GPIO_MODE_USER_STD_OUTPUT
//      GPIO_MODE_USER_STD_BIDIRECTIONAL
//      GPIO_MODE_USER_STD_ANALOG


//  ======= set each IO to the desired configuration =============

    //  GPIO 0 is turned off to prevent toggling the debug pin; For debug, make this an output and
    //  drive it externally to ground.

    reg_mprj_io_0 = GPIO_MODE_MGMT_STD_ANALOG;

    // Changing configuration for IO[1-4] will interfere with programming flash. if you change them,
    // You may need to hold reset while powering up the board and initiating flash to keep the process
    // configuring these IO from their default values.

    reg_mprj_io_1 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_2 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_3 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_4 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;

    // -------------------------------------------
    reg_mprj_io_29 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_30 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_31 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_32 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_33 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_34 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_35 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_36 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out

    // Initiate the serial transfer to configure IO
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}

void delay(const int d)
{

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

void main()
{
	int i, j, k;

    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oe = 1;

    configure_io();

	while (1) {
        reg_gpio_out = 1; // OFF
        reg_mprj_datah = 0x0000001F; // lower 5 bits
        reg_mprj_datal = 0xE0000000; // top 3 bits 
		delay(1000000);
        reg_gpio_out = 0; // ON
		delay(1000000);
        reg_mprj_datah = 0x00000000; // lower 5 bits
        reg_mprj_datal = 0x00000000; // top 3 bits 

    }
}
