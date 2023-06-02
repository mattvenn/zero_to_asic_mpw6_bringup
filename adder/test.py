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

with serial.Serial(port, 9600, timeout=1) as ser:
    ser.write(b'p')
    resp = ser.readline()
    assert resp == b'choose project 2 -> 6: '
    ser.write(b'2')
    resp = ser.readline()
    assert resp == b'set project to  002\r\n'
    
    for i in range(10):
        a = random.randint(0, 2**32)
        b = random.randint(0, 2**32)
        result = (a + b) % (2**32)
        print(f"{a:08x} + {b:08x} = {result:08x}")
        assert add(a, b) == result
