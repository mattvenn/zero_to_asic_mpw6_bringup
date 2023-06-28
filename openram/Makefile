
# Set the prefix for `riscvXX-unknown-elf-*`
# On installations using `multilib`, this will be `riscv64` even for compiling to 32-bit targets
TOOLCHAIN_PREFIX=riscv64-unknown-elf
#TOOLCHAIN_PREFIX=riscv32

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = testram

hex:  ${PATTERN:=.hex}

CFLAGS=-c -O0 -mabi=ilp32 -march=rv32i -D__vexriscv__  -ffreestanding -nostdlib
LDFLAGS=-lm -Wl,-Bstatic,-T,../sections.lds,--strip-debug -nostdlib
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

.c.o:
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -I../ -I../generated/ $(FLAGS) $< -o  $@

.o.: ../sections.lds ../crt0_vex.S ../isr.c ../stub.c
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc $^ $(LDFLAGS) -o $@

%.elf: %.o
#	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -I../ -I../generated/ -O0 -mabi=ilp32 -march=rv32i -D__vexriscv__ -Wl,-Bstatic,-T,../sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ ../crt0_vex.S ../isr.c ../stub.c $<
	${TOOLCHAIN_PATH}$(TOOLCHAIN_PREFIX)-objdump -D testram.elf > testram.lst

%.hex: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O verilog $< $@
	sed -ie 's/@1000/@0000/g' $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

flash: testram.hex
	python3 ../util/caravel_hkflash.py testram.hex

flash2: testram.hex
	python3 ../util/caravel_flash.py testram.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd

.PHONY: clean hex all flash

