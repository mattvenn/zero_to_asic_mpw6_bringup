#!/usr/bin/env python3
import random
import serial
port = '/dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00'

def write_mem(addr, val):
    ser.write(f"W{addr:02x}:{val:08x}\n".encode())
    return int(ser.readLine().decode(), 16)

def read_mem(addr):
    ser.write(f"R{addr:02x}\n".encode())
    return int(ser.readLine().decode(), 16)

def set_latencies(lat):
    ser.write(f"L{lat:08x}".encode())
    return int(ser.readLine().decode(), 16)

def get_latencies():
    ser.write("l\n")
    return int(ser.readLine().decode(), 16)

def set_mgmt_RW():
    ser.write("M\n")
    ser.readLine()

def set_uprj_RW():
    ser.write("U\n")
    ser.readLine()

with serial.Serial(port, 9600, timeout=1) as ser:
    set_mgmt_RW()
    
    latencies = 0x04050405
    assert set_latencies(latencies) == latencies
    assert get_latencies() == latencies

    latencies = 0x03030303
    assert set_latencies(latencies) == latencies
    assert get_latencies() == latencies

    cells = 256
    errs = 0
    for addr in range(cells):
        value = random.randint(0, 2**32)
        if (write_mem(addr, value) != value):
            errs += 1
        elif (read_mem(addr) != value):
            errs += 1
    
    print(f"RW test completed with {errs} errors")
