#include <defs.h>
#include <stub.h>

#define PROJECT_ID 7

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
    reg_mprj_io_8 =   GPIO_MODE_USER_STD_INPUT_NOPULL; // select
    reg_mprj_io_9 =   GPIO_MODE_USER_STD_INPUT_NOPULL; // restart
    reg_mprj_io_10 =  GPIO_MODE_USER_STD_INPUT_NOPULL; // rotary a
    reg_mprj_io_11 =  GPIO_MODE_USER_STD_INPUT_NOPULL; // rotary b 

    reg_mprj_io_12 =  GPIO_MODE_USER_STD_OUTPUT; // seven seg 
    reg_mprj_io_13 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_14 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_15 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_16 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_17 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_18 =  GPIO_MODE_USER_STD_OUTPUT; // seven seg
    reg_mprj_io_19 =  GPIO_MODE_USER_STD_OUTPUT; // seven seg digit
    reg_mprj_io_20 =  GPIO_MODE_USER_STD_OUTPUT; // led flag
    reg_mprj_io_21 =  GPIO_MODE_USER_STD_OUTPUT; // sync

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
	//while (1) {
    {
        reg_gpio_out = 1; // OFF
		delay(4000000);
        reg_gpio_out = 0; // ON
		delay(4000000);
    }

    //PLL test
    // https://caravel-harness.readthedocs.io/en/latest/housekeeping-spi.html
    /* from defs.h
    #define reg_hkspi_pll_ena     (*(volatile uint32_t*)0x2610000c) = 0x08
    #define reg_hkspi_pll_bypass  (*(volatile uint32_t*)0x26100010) = 0x09
    #define reg_hkspi_pll_trim    (*(volatile uint32_t*)0x2610001c) = 0x0D -> 0x10
    #define reg_hkspi_pll_source  (*(volatile uint32_t*)0x26100020) = 0x11
    #define reg_hkspi_pll_divider (*(volatile uint32_t*)0x26100024) = 0x12
    */

    reg_hkspi_pll_ena = 1; // pll enable & pll dco disable

    // try to get 20mhz
    reg_hkspi_pll_source = 0x3f;
    reg_hkspi_pll_divider = 0x0e;
    // try to get 30mhz
//    reg_hkspi_pll_source = 0x2d;
 //   reg_hkspi_pll_divider = 0x0f;

    reg_hkspi_pll_bypass = 0; // use the pll to drive the clock (and hence the designs)

	while (1) 
    {
        reg_gpio_out = 1; // OFF
		delay(4000000);
        reg_gpio_out = 0; // ON
		delay(4000000);
    }

}
