
#segments = [ 63, 6, 91, 79, 102, 109, 124, 7, 127, 103 ]
segments = [ 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71]
count = 0
for digit in segments:
    lower = digit & 0b00000111
    upper = digit & 0b11111000
#    print(bin(lower), bin(upper))
    lower = lower << 29
    upper = upper >> 3
    #print(f"reg_mprj_datah = {hex(upper)}; reg_mprj_datal = {hex(lower)}; delay(DELAY);")
    print(f"case {count:2d}: reg_mprj_datah = {hex(upper)}; reg_mprj_datal = {hex(lower)}; break;")
    count += 1;
