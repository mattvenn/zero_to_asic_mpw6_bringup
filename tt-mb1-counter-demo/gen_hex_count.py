
segments = [ 63, 6, 91, 79, 102, 109, 124, 7, 127, 103 ]
for digit in segments:
    lower = digit & 0b00000111
    upper = digit & 0b11111000
#    print(bin(lower), bin(upper))
    lower = lower << 29
    upper = upper >> 3
    print(f"reg_mprj_datah = {hex(upper)}; reg_mprj_datal = {hex(lower)}; delay(DELAY);")
