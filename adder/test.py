#!/usr/bin/env python3
import random
import serial
port = '/dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00'

def add(a, b):
    ser.write(b'a')
    resp = ser.readline()
    assert resp == b'waiting for 8 characters\r\n'
    ser.write(f"{a:08x}".encode())
    resp = ser.readline()
    assert resp == b'waiting for 8 characters\r\n'
    ser.write(f"{b:08x}".encode())
    resp = int(ser.readline().decode(), 16)
    return resp

def select_project(p):
    ser.write(b'p')
    resp = ser.readline()
    assert resp == b'choose project 2 -> 6: '
    ser.write(str(p).encode())
    resp = ser.readline()
    assert resp == f'set project to  00{p}\r\n'.encode()
    print(f"set project to {p}")

with serial.Serial(port, 9600, timeout=1) as ser:
    for project in [2,3,4,5,6]:
        select_project(project)
    
        num_tests = 1000
        for i in range(num_tests):
            a = random.randint(0, 2**32)
            b = random.randint(0, 2**32)
            result = (a + b) % (2**32)
            #print(f"{a:08x} + {b:08x} = {result:08x}")
            assert add(a, b) == result
        print(f"completed {num_tests} tests")
