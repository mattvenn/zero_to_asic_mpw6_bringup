#ifndef OPENRAM_H
#define OPENRAM_H

#include <stdint.h>

enum RAM_PORT_CONFIG {
    RAM_CONF__MGMT_RW__UPRJ_RO = 0,
    RAM_CONF__MGMT_RO__UPRJ_RW = 1
};

void set_ram_port_config(enum RAM_PORT_CONFIG config);

void set_ram_latencies(uint8_t mgmt_prefetch, uint8_t mgmt_read, uint8_t uprj_prefetch, uint8_t uprj_read);
uint32_t read_ram_latencies();

void write_to_ram(uint8_t addr, uint32_t data);
uint32_t read_from_ram(uint8_t addr);

#endif // OPENRAM_H