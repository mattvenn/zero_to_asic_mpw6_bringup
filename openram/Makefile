
# Set the prefix for `riscvXX-unknown-elf-*`
# On installations using `multilib`, this will be `riscv64` even for compiling to 32-bit targets
TOOLCHAIN_PREFIX=riscv64-unknown-elf

PROJECT = testram
LD_SCRIPT = ../sections.lds
OUT_DIR = generated

INC_DIRS = .. ../generated
SRCS_DIRS = ..
LIB_DIRS =

C_EXT = c
C_SRCS = $(wildcard $(patsubst %, %/*.$(C_EXT), . $(SRCS_DIRS)))
AS_EXT = S
AS_SRCS = $(wildcard $(patsubst %, %/*.$(AS_EXT), . $(SRCS_DIRS)))


OPTIMIZATION = -O0 -ffunction-sections -fdata-sections

# when using output folder, append trailing slash to its name
ifeq ($(strip $(OUT_DIR)), )
	OUT_DIR_F =
else
	OUT_DIR_F = $(strip $(OUT_DIR))/
endif

VPATH = $(SRCS_DIRS)

# flags
CORE_FLAGS = -mabi=ilp32 -march=rv32i -D__vexriscv__
CFLAGS = -ffreestanding -nostdlib
AS_FLAGS = -Wa,-amhls=$(OUT_DIR_F)$(notdir $(<:.$(AS_EXT)=.lst))
LD_FLAGS = -Wl,-Bstatic,-T$(LD_SCRIPT),--strip-debug,--gc-sections,-Map=$(OUT_DIR_F)$(PROJECT).map,--cref,--no-warn-mismatch -ffreestanding -nostdlib

# intermediary files
C_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(C_SRCS:.$(C_EXT)=.o)))
AS_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(AS_SRCS:.$(AS_EXT)=.o)))
OBJS = $(AS_OBJS) $(C_OBJS)
DEPS = $(OBJS:.o=.d)
INC_DIRS_F = -I. $(patsubst %, -I%, $(INC_DIRS))
LIB_DIRS_F = $(patsubst %, -L%, $(LIB_DIRS))

# output files generated
ELF = $(OUT_DIR_F)$(PROJECT).elf
HEX = $(OUT_DIR_F)$(PROJECT).hex
BIN = $(OUT_DIR_F)$(PROJECT).bin
LSS = $(OUT_DIR_F)$(PROJECT).lss
DMP = $(OUT_DIR_F)$(PROJECT).dmp

# final flags
C_FLAGS_F = $(CORE_FLAGS) $(OPTIMIZATION) $(C_FLAGS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
AS_FLAGS_F = $(CORE_FLAGS) $(AS_FLAGS) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
LD_FLAGS_F = $(CORE_FLAGS) $(LD_FLAGS) $(LIB_DIRS_F)

# output directory
GENERATED = $(wildcard $(patsubst %, $(OUT_DIR_F)*.%, bin d dmp elf hex lss lst map o))

all : make_output_dir $(ELF) $(LSS) $(DMP) $(HEX) $(BIN)

# object files dependent on Makefile
$(OBJS) : Makefile
# elf dependent on linker script
$(ELF) : $(LD_SCRIPT)

$(ELF) : $(OBJS)
	@echo 'Linking target: $(ELF)'
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc $(LD_FLAGS_F) $(OBJS) $(LIBS) -o $@
	@echo ' '

$(OUT_DIR_F)%.o : %.$(C_EXT)
	@echo 'Compiling $<'
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -c $(C_FLAGS_F) $< -o $@
	@echo ' '

$(OUT_DIR_F)%.o : %.$(AS_EXT)
	@echo 'Assembling $<'
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -c $(AS_FLAGS_F) $< -o $@
	@echo ' '

$(HEX) : $(ELF)
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O verilog $< $@
	sed -ie 's/@1000/@0000/g' $@
	@echo ' '

$(BIN) : $(ELF)
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O binary $< $@
	@echo ' '

$(DMP) : $(ELF)
	@echo 'Creating memory dump: $(DMP)'
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objdump -x --syms $< > $@
	@echo ' '

$(LSS) : $(ELF)
	@echo 'Creating extended listing: $(LSS)'
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objdump -S $< > $@
	@echo ' '

make_output_dir :
	$(shell mkdir $(OUT_DIR_F) 2>/dev/null)

flash: $(PROJECT).hex
	python3 ../util/caravel_hkflash.py $(PROJECT).hex

flash2: $(PROJECT).hex
	python3 ../util/caravel_flash.py $(PROJECT).hex


clean:
	rm -f $(GENERATED)

.PHONY: clean hex all flash dependents
.SECONDARY:

-include $(DEPS)
