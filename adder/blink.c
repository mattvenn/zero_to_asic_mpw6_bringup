#include <defs.h>
//#include <math.h>
#include <stub.h>
#include <hw/common.h>
#include <uart.h>
#include <uart_api.h>

/*
2: behavioural
3: sklansky
4: Brent Kung
5: ripple 
6: kogge 
*/
//#define PROJECT_ID 5

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

int wait_for_char()
{
    int uart_temp;
    while (uart_rxempty_read() == 1);
    uart_temp = reg_uart_data;
   
    uart_pop_char();
    return uart_temp;
}

void set_mux(unsigned char reg_sel, unsigned int value)
{
    reg_la1_data = (reg_la1_data & ~REG_SEL_MASK) | ((reg_sel << 9) & REG_SEL_MASK);
    reg_la3_data = value;
    SET(reg_la1_data, MUX_WRITE);
    CLR(reg_la1_data, MUX_WRITE);
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
    reg_mprj_io_8 = GPIO_MODE_USER_STD_OUTPUT; // stop
    reg_mprj_io_9 = GPIO_MODE_USER_STD_OUTPUT; // ring out
    reg_mprj_io_10 = GPIO_MODE_USER_STD_OUTPUT; // done

    reg_mprj_io_11 = GPIO_MODE_MGMT_STD_OUTPUT; // fw ready
    reg_mprj_io_12 = GPIO_MODE_MGMT_STD_OUTPUT; // fw done

    reg_mprj_io_13 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_14 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_15 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_16 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_17 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_18 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_19 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out
    reg_mprj_io_20 = GPIO_MODE_MGMT_STD_OUTPUT; // ring osc counter out

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

unsigned long pow(int a, int b)
{
    unsigned long result = a;
    while (b != 0) {
        result *= 16;
        --b;
    }
    return result;
}
unsigned long read32bit()
{
    unsigned long a = 0;
    char c = 0;
    int i = 0;
    print("waiting for 8 characters\n");
    for(i = 0; i < 8; i++)
    {
        c = wait_for_char() - 48;
        if(c > 10)
            c -= 39;
        a += pow(c, 7-i);
    }
    return a;
}

void test_adder_in_ring(int bits)
{
    // hold in reset
    SET(reg_la1_data, RESET);
    // stop the ring
    CLR(reg_la1_data, STOP_B);

    if(bits)
    {
    // setup for 2 bit
    // set a & b input to be 0
    set_mux(A_INPUT, 0x0006); // 110
    set_mux(B_INPUT, 0x0001); // 001

    // disable extra inverter
    CLR(reg_la1_data, EXTRA_INV);

    // set control pins up include adder
    set_mux(A_INPUT_EXT_BIT,     0xFFFFFFFF ^ 0x0006);      // which bits to allow through from a input
    set_mux(A_INPUT_RING_BIT,    0xFFFFFFFF ^ 1 << 0 );     // which bit the ring enters the a input of adder
    set_mux(S_OUTPUT_BIT,        0xFFFFFFFF ^ 1 << 2 );     // which bit of the adder's sum goes back to the ring
    }
    else
    {

    // setup for 0 bit
    // set a & b input to be 0
    set_mux(A_INPUT, 0x0000); // 110
    set_mux(B_INPUT, 0x0000); // 001

    // disable extra inverter
    SET(reg_la1_data, EXTRA_INV);

    // set control pins up include adder
    set_mux(A_INPUT_EXT_BIT,     0xFFFFFFFF ^ 0x0000);      // which bits to allow through from a input
    set_mux(A_INPUT_RING_BIT,    0xFFFFFFFF ^ 1 << 0 );     // which bit the ring enters the a input of adder
    set_mux(S_OUTPUT_BIT,        0xFFFFFFFF ^ 1 << 0 );     // which bit of the adder's sum goes back to the ring
    }

    // disable control loop
    SET(reg_la1_data, CONTROL_B);

    // disable bypass loop
    SET(reg_la1_data, BYPASS_B);

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
    print("0x");
    print_hex(reg_la2_data_in, 8);
    print("\n");

}

unsigned long test_adder(unsigned long a, unsigned long b)
{
        set_mux(A_INPUT, a);    // set input a
        set_mux(B_INPUT, b);    // set input b
        set_mux(SUM, 0);        // 0 is ignored

        print_hex(reg_la3_data_in, 8);
        print("\n");
}

void test_ring_osc(int control, int print_result) 
{
    // tell the test bench we're ready
    reg_mprj_datal |= 1 << FW_READY;


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

void main()
{
	int i, j, k;

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

    unsigned long a;
    unsigned long b;

	while (1) {
        reg_gpio_out = 1; // OFF
		delay(1000000);
        reg_gpio_out = 0; // ON
		delay(1000000);

        char c = wait_for_char();
        switch(c) {
            case 'a':
                a = read32bit();
                b = read32bit();
                test_adder(a, b);
                break;
            case 'p':
                print("choose project 2 -> 6: ");
                c = wait_for_char();
                print("set project to ");
                print_dec(c-48);
                print("\n");
                reg_la0_data = (1 << (c-48)); // enable the project
                break;
            case 'c':
                print("running 200 then 20x control\n");
                for(i = 0 ; i < 200; i ++)
                {
                    print(".");
                    test_ring_osc(1, 0);
                }
                print("\n");
                for(i = 0 ; i < 20; i ++)
                    test_ring_osc(1, 1);
                print("done\n");
                break;
            case 'b':
                print("running 200 then 20x bypass\n");
                for(i = 0 ; i < 200; i ++)
                {
                    print(".");
                    test_ring_osc(0, 0);
                }
                print("\n");
                for(i = 0 ; i < 20; i ++)
                    test_ring_osc(0, 1);
                print("done\n");
                break;
            case 't':
                print("running 1x control\n");
                test_ring_osc(1, 1);
                break;
            case 'u':
                print("test adder in ring\n");
                print("running 20x adder\n");
                for(i = 0 ; i < 20; i ++)
                    test_adder_in_ring(0);
                print("done\n");
                break;
            case 'v':
                print("test adder bits in ring\n");
                print("running 20x adder\n");
                for(i = 0 ; i < 20; i ++)
                    test_adder_in_ring(1);
                print("done\n");
                break;
            default:
                print("p: select project\nb: run with bypass\nc: run with control\nu: test adder in ring\n");
                break;
        }
    }
}
