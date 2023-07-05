#include <defs.h>
#include <stub.h>
#include <hw/common.h>
#include <uart.h>
#include <uart_api.h>

uint8_t serial_getc()
{
    uint8_t rcvd;

    while (uart_rxempty_read() == 1);
    rcvd = reg_uart_data;
   
    uart_ev_pending_write(UART_EV_RX);

    return rcvd;
}

void serial_putc(uint8_t data)
{
    while (reg_uart_txfull == 1);

    reg_uart_data = data;
}

uint32_t serial_gethex(uint8_t eol)
{
    uint32_t val = 0;
    uint8_t rcvd;
    while ((rcvd = serial_getc()) != eol)
    {
        rcvd -= (rcvd > '9') ? ('a'-10) : '0';
        val <<= 4;
        val |= rcvd;
    }

    return val;
}
