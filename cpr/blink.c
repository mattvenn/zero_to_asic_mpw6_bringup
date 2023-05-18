#include <defs.h>
#include <stub.h>

#define PROJECT_ID 12

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

    // 1 inputs | Start
    reg_mprj_io_8 =   GPIO_MODE_USER_STD_INPUT_NOPULL;

    // 3 outputs | Compress Out, Breath Out and Pulse Out
    reg_mprj_io_14 =  GPIO_MODE_USER_STD_OUTPUT; // compress
    reg_mprj_io_15 =  GPIO_MODE_USER_STD_OUTPUT; // breath
    reg_mprj_io_16 =  GPIO_MODE_USER_STD_OUTPUT; // pulse
    // Reset    
    reg_mprj_io_17 =  GPIO_MODE_USER_STD_OUTPUT; // sync

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

    //reg_uart_enable = 1;

    // Configure All LA probes as inputs to the cpu
	//reg_la0_oenb = reg_la0_iena = 0x00000000;    // [31:0]
	//reg_la1_oenb = reg_la1_iena = 0x00000000;    // [63:32]
	////reg_la2_oenb = reg_la2_iena = 0x00000000;    // [95:64]
	//reg_la3_oenb = reg_la3_iena = 0x00000000;    // [127:96]

	// write data to la output
    //	reg_la0_data = 0x00;
    //	reg_la1_data = 0x00;
    //	reg_la2_data = 0x00;
    //	reg_la3_data = 0x00;

    // read data from la input
    //	data0 = reg_la0_data;
    //	data1 = reg_la1_data;
    //	data2 = reg_la2_data;
    //	data3 = reg_la3_data;

    // activate the project by setting the 0th bit of 1st bank of LA
    reg_la0_iena = 0; // input enable off
    reg_la0_oenb = 0xFFFFFFFF; // enable all of bank0 logic analyser outputs (ignore the name, 1 is on, 0 off)
    reg_la0_data = (1 << PROJECT_ID); // enable the project

    // reset design with 0bit of 2nd bank of LA
    reg_la1_oenb = 0xFFFFFFFF; // enable
    reg_la1_iena = 0;
    reg_la1_data = 1;
    delay(8000000);
    reg_la1_data = 0;

    // blink GPIO
	while (1) {
        reg_gpio_out = 1; // OFF
		delay(4000000);
        reg_gpio_out = 0; // ON
		delay(4000000);
    }
}
