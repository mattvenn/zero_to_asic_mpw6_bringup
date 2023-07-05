#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>

uint8_t serial_getc();
void serial_putc(uint8_t data);
uint32_t serial_gethex(uint8_t eol);

#endif // _SERIAL_H