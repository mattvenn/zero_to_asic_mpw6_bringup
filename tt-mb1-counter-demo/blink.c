#include <defs.h>
//#include <math.h>
#include <stub.h>
#include <hw/common.h>
#include <uart.h>
#include <uart_api.h>

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

// input control pins
#define RESET           0
#define STOP_B          1
#define EXTRA_INV       2
#define BYPASS_B        3
#define CONTROL_B       4
#define COUNTER_EN      5
#define COUNTER_LOAD    6 
#define COUNT_FORCE     7 

// mux control
#define MUX_WRITE       8
#define REG_SEL_0       9  
#define REG_SEL_1       10
#define REG_SEL_2       11
#define REG_SEL_3       12

// ring osc starting pin
#define RING_OUT_BIT0   13

// output status pin
#define DONE            0

#define FW_READY        11
#define FW_DONE         12

#define SET(PIN,N) (PIN |=  (1<<N))
#define CLR(PIN,N) (PIN &= ~(1<<N))
#define GET(PIN,N) (PIN &   (1<<N))

// mux
#define REG_SEL_MASK    0x1E00
#define A_INPUT           0
#define B_INPUT           1
#define S_OUTPUT_BIT      2
#define A_INPUT_EXT_BIT   3
#define A_INPUT_RING_BIT  4
#define SUM               5

void configure_io()
{

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

    // design select
    reg_mprj_io_12 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_13 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_14 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_15 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_16 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_17 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_18 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_19 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;
    reg_mprj_io_20 = GPIO_MODE_MGMT_STD_INPUT_PULLDOWN;

    // inputs
    reg_mprj_io_21 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_22 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_23 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_24 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_25 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_26 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_27 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;
    reg_mprj_io_28 = GPIO_MODE_MGMT_STD_INPUT_PULLUP;

    // pins for the 7 seg counter
    reg_mprj_io_29 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_30 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_31 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_32 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_33 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_34 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_35 = GPIO_MODE_MGMT_STD_OUTPUT; 
    reg_mprj_io_36 = GPIO_MODE_MGMT_STD_OUTPUT; 

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

int get_inputs()
{
    return (reg_mprj_datal >> 21) & 0xff;
}

#define DELAY 5000000

void set_mux(unsigned char reg_sel, unsigned int value)
{
    reg_la1_data = (reg_la1_data & ~REG_SEL_MASK) | ((reg_sel << 9) & REG_SEL_MASK);
    reg_la3_data = value;
    SET(reg_la1_data, MUX_WRITE);
    CLR(reg_la1_data, MUX_WRITE);
}

void test_ring_osc(int control, int print_result) 
{
    // tell the test bench we're ready
//    reg_mprj_datal |= 1 << FW_READY;


    // reproduce test_bypass in the test_adder.py to measure the ring oscillator
    // it doesn't work here for some reason. The ring never resolves from x
    // the bypass input is defined, but the output of the first bypass tristate is always x

    // hold in reset
    SET(reg_la1_data, RESET);
    // stop the ring
    CLR(reg_la1_data, STOP_B);
    // disable extra inverter
    SET(reg_la1_data, EXTRA_INV);

    // todo
    // set a & b input to be 0
    set_mux(A_INPUT, 0);
    set_mux(B_INPUT, 0);
    // set control pins up to bypass adder
    set_mux(A_INPUT_EXT_BIT,     0x0);
    set_mux(S_OUTPUT_BIT,        0xFFFFFFFF);
    set_mux(A_INPUT_RING_BIT,    0xFFFFFFFF);

    // enable control loop
    if(control)
    {
        // enable control loop
        CLR(reg_la1_data, CONTROL_B);
        SET(reg_la1_data, BYPASS_B);
    }
    else
    {
        // enable bypass loop
        SET(reg_la1_data, CONTROL_B);
        CLR(reg_la1_data, BYPASS_B);
    }


    // load the integration counter
    reg_la2_data = 2000000;
    SET(reg_la1_data, COUNTER_LOAD);
    CLR(reg_la1_data, RESET);
    CLR(reg_la1_data, COUNTER_LOAD);

    // start the loop & enable in the same cycle
    reg_la1_data |= ((1 << STOP_B) | (1 << COUNTER_EN));

    // wait for done to go high
    while(1) 
    {
        if(GET(reg_la1_data_in, DONE))
            break;
    }

    // set the ring osc value onto the pins
    //reg_mprj_datal = reg_la2_data_in << RING_OUT_BIT0;

    // set done on the mprj pins
//    reg_mprj_datal |= 1 << FW_DONE;
    if(print_result)
    {
    print("0x");
    print_hex(reg_la2_data_in, 8);
    print("\n");
    }

}

void show_digit(char digit)
{
    switch(digit)
    {
        // these values generated by gen_hex_count.py
        case  0: reg_mprj_datah = 0x7; reg_mprj_datal = 0xe0000000; break;
        case  1: reg_mprj_datah = 0x0; reg_mprj_datal = 0xc0000000; break;
        case  2: reg_mprj_datah = 0xb; reg_mprj_datal = 0x60000000; break;
        case  3: reg_mprj_datah = 0x9; reg_mprj_datal = 0xe0000000; break;
        case  4: reg_mprj_datah = 0xc; reg_mprj_datal = 0xc0000000; break;
        case  5: reg_mprj_datah = 0xd; reg_mprj_datal = 0xa0000000; break;
        case  6: reg_mprj_datah = 0xf; reg_mprj_datal = 0xa0000000; break;
        case  7: reg_mprj_datah = 0x0; reg_mprj_datal = 0xe0000000; break;
        case  8: reg_mprj_datah = 0xf; reg_mprj_datal = 0xe0000000; break;
        case  9: reg_mprj_datah = 0xc; reg_mprj_datal = 0xe0000000; break;
        case 10: reg_mprj_datah = 0xe; reg_mprj_datal = 0xe0000000; break;
        case 11: reg_mprj_datah = 0xf; reg_mprj_datal = 0x80000000; break;
        case 12: reg_mprj_datah = 0x7; reg_mprj_datal = 0x20000000; break;
        case 13: reg_mprj_datah = 0xb; reg_mprj_datal = 0xc0000000; break;
        case 14: reg_mprj_datah = 0xf; reg_mprj_datal = 0x20000000; break;
        case 15: reg_mprj_datah = 0xe; reg_mprj_datal = 0x20000000; break;
    }
}

void main()
{
    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oe = 1;

    configure_io();

    reg_uart_enable = 1;

    // activate the project by setting the 0th bit of 1st bank of LA
    reg_la0_iena = 0; // input enable off
    reg_la0_oenb = 0xFFFFFFFF; // enable all of bank0 logic analyser outputs (ignore the name, 1 is on, 0 off)

    reg_la1_oenb = 0xFFFFFFFF; // enable
    reg_la1_iena = 0;          // enable

    reg_la2_oenb = 0xFFFFFFFF; // enable
    reg_la2_iena = 0;          // enable

    reg_la3_oenb = 0xFFFFFFFF; // enable
    reg_la3_iena = 0;


    // blink
    reg_gpio_out = 1; // OFF
    delay(4000000);
    reg_gpio_out = 0; // ON
    delay(4000000);
    print("started\n");

    char design = 0;
    char count = 0;
    char inputs = 0;
	while (1) 
    {
        design = reg_mprj_datal >> 12;
        print("design: ");
        print_hex(design, 1);
        print("\n");
        inputs = get_inputs();

        switch(design) 
        {
            case 0:
                // blink led
                reg_gpio_out = 1; // OFF
                delay(1000000);
                reg_gpio_out = 0; // ON
                delay(1000000);
                break;
            case 1:
                //do the counter
                count ++;
                print_hex(count, 1);
                show_digit(count & 0xf);
                print("input: ");
                print_hex(inputs, 2);
                print("\n");
                delay((inputs + 1)* 312500l);
                break;
            case 2:
                // do the ring osc
                // set project to 2 (behavioural adder)
                reg_la0_data = (1 << 2); // enable the project
                test_ring_osc(1, 1);
                char temp = (reg_la2_data_in >> (inputs + 8)) & 0xf; // shifted by 10 works ok
                print("input: ");
                print_hex(get_inputs(), 2);
                print("\n");
                print("7seg: ");
                print_hex(temp, 2);
                print("\n");
                show_digit(temp);
                break;
        }
    }
}
