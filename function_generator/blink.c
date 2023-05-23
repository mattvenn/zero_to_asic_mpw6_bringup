#include <defs.h>
#include <stub.h>

#define PROJECT_ID 0

#define REG_CONFIG                  (*(volatile uint32_t*)0x30000000)
#define SRAM_LATENCY_CSR_BASE_ADDR  0x300FF800
#define SRAM_BASE_ADDR              0x300FFC00
#define OPENRAM(addr)               (*(volatile uint32_t*)(SRAM_BASE_ADDR + (addr & 0x3fc)))
#define OPENRAM_LATENCY         (*(volatile uint32_t*)(SRAM_LATENCY_CSR_BASE_ADDR))
#define SRAM_WRITE_PORT         31  // last bit of the 1st bank logic analyser. If set to 0, Caravel can write to shared RAM

void config_generator(uint16_t period, uint8_t end_addr, bool run)
{
    REG_CONFIG = (run << 24) + (end_addr << 16) + period;
}

void write_to_ram(uint8_t addr, uint32_t data)
{
    OPENRAM(addr << 2) = data;
}

uint32_t read_from_ram(uint8_t addr)
{
    return OPENRAM(addr << 2);
}

void set_ram_latencies(uint8_t mgmt_pre, uint8_t mgmt_read, uint8_t uprj_pre, uint8_t uprj_read)
{
    OPENRAM_LATENCY =   (mgmt_pre << 24) 
                | (mgmt_read << 16) 
            | (uprj_pre << 8) 
            | uprj_read;   
}

uint32_t read_ram_latencies()
{
    return OPENRAM_LATENCY;
}
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
    // 8 outputs for DAC
    reg_mprj_io_8  =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_9  =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_10 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_11 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_12 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_13 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_14 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_15 =   GPIO_MODE_USER_STD_OUTPUT;

    // debug
    reg_mprj_io_16 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_17 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_18 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_19 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_20 =   GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_21 =   GPIO_MODE_USER_STD_OUTPUT;

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

    // activate the project by setting the 0th bit of 1st bank of LA
    reg_la0_iena = 0; // input enable off
    reg_la0_oenb = 0xFFFFFFFF; // enable all of bank0 logic analyser outputs (ignore the name, 1 is on, 0 off)

    // reset design with 0bit of 2nd bank of LA
    reg_la1_oenb = 0xFFFFFFFF; // enable
    reg_la1_iena = 0;
    reg_la1_data = 1;
    delay(8000000);
    reg_la1_data = 0;



    // enable wishbone
    reg_wb_enable  = 1;

    // allow Caravel to write to the shared RAM
    reg_la0_data &= ~(1 << SRAM_WRITE_PORT); 

    // configure latencies to 3,3,3,3
    set_ram_latencies(3,3,3,3);

    // load the function data into sram
    for(i = 0; i < 64; i ++)
    {
        write_to_ram(i, 4*i + ((4*i+1) << 8) + ((4*i+2) << 16) + ((4*i+3) << 24));
    }

    // read 1 address back just to check
    read_from_ram(0);

    // activate the project by setting the 1st bit of 1st bank of LA - depends on the project ID
    reg_la0_data |= (1 << PROJECT_ID);
    //reg_la0_data = (1 << PROJECT_ID); // enable the project

    // configure function generator: 10 clock cycles per sample, 64 * 4 samples, start run
    config_generator(4, 64, 1);

    // blink GPIO
	while (1) {
        reg_gpio_out = 1; // OFF
		delay(4000000);
        reg_gpio_out = 0; // ON
		delay(4000000);
    }
}
