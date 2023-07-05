
#include <defs.h>

#include "openram.h"

#define REG_CONFIG                  (*(volatile uint32_t*)0x30000000)
#define SRAM_LATENCY_CSR_BASE_ADDR  0x300FF800
#define SRAM_BASE_ADDR              0x300FFC00
#define OPENRAM(addr)               (*(volatile uint32_t*)(SRAM_BASE_ADDR + (addr & 0x3fc)))
#define OPENRAM_LATENCY             (*(volatile uint32_t*)(SRAM_LATENCY_CSR_BASE_ADDR))

#define SRAM_WRITE_PORT_BIT         31  // last bit of the 1st bank logic analyser. If set to 0, Caravel can write to shared RAM
#define SRAM_WRITE_PORT_MASK        ((1ul << SRAM_WRITE_PORT_BIT))


void set_ram_port_config(enum RAM_PORT_CONFIG config) {
    switch (config) {
        case RAM_CONF__MGMT_RW__UPRJ_RO: 
            // allow Caravel to write to the shared RAM
            reg_la0_data &= ~SRAM_WRITE_PORT_MASK;
            break;
        case RAM_CONF__MGMT_RO__UPRJ_RW:
            // allow user project to write to the shared RAM
            reg_la0_data |= SRAM_WRITE_PORT_MASK;
            break;
        default:
            // ERROR!
            break;
    }
}

void write_to_ram(uint8_t addr, uint32_t data) {
    OPENRAM(addr << 2) = data;
}

uint32_t read_from_ram(uint8_t addr) {
    return OPENRAM(addr << 2);
}

void set_ram_latencies(uint8_t mgmt_prefetch, uint8_t mgmt_read, uint8_t uprj_prefetch, uint8_t uprj_read) {
    OPENRAM_LATENCY =   (mgmt_prefetch << 24) | (mgmt_read << 16) 
                        | (uprj_prefetch << 8) | uprj_read;   
}

void set_ram_latencies_w(uint32_t latencies) {
    OPENRAM_LATENCY = latencies;
}

uint32_t read_ram_latencies_w() {
    return OPENRAM_LATENCY;
}