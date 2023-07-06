#!/usr/bin/env python3
import random
import serial
port = '/dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00'

def write_mem(addr, val):
    ser.write(f"W{addr:02x}:{val:08x}\n".encode())
    return int(ser.readline().decode(), 16)

def read_mem(addr):
    ser.write(f"R{addr:02x}\n".encode())
    return int(ser.readline().decode(), 16)

def set_latencies(lat):
    print(f"L{lat:08x}")
    ser.write(f"L{lat:08x}\n".encode())
    return int(ser.readline().decode(), 16)

def get_latencies():
    ser.write("l\n".encode())
    return int(ser.readline().decode(), 16)

def set_mgmt_RW():
    ser.write("M\n".encode())
    ser.readline()

def set_uprj_RW():
    ser.write("U\n")
    ser.readline()

with serial.Serial(port, 9600, timeout=1) as ser:
    set_mgmt_RW()
    
    latencies = 0x04050405
    assert set_latencies(latencies) == latencies
    assert get_latencies() == latencies
    print(f"latency {latencies:08x} ok") 

    latencies = 0x03030303
    assert set_latencies(latencies) == latencies
    assert get_latencies() == latencies
    print(f"latency {latencies:08x} ok") 


    cells = 256
    errs = 0
    print("starting cell test")
    for addr in range(cells):
        if addr % 16 == 0:
            print(f"{addr:08} errs {errs}")
        value = random.randint(0, 2**32)
        if (write_mem(addr, value) != value):
            errs += 1
        elif (read_mem(addr) != value):
            errs += 1
    
    print(f"RW test completed with {errs} errors")
