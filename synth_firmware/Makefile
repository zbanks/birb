# Toolchain
TOOLCHAIN=/home/eric/avr-test/sandbox/avr8-gnu-toolchain-linux_x86_64/bin/
ARCH_PREFIX=avr-
CC=$(TOOLCHAIN)$(ARCH_PREFIX)gcc
LD=$(TOOLCHAIN)$(ARCH_PREFIX)gcc
OBJCOPY=$(TOOLCHAIN)$(ARCH_PREFIX)objcopy
BIN2SYX=bl/bin2syx.py
AMIDI=amidi

# Files
C_SRC  = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(C_SRC))

# Assembler, compiler, and linker flags
CFLAGS = -B /home/eric/avr-test/sandbox/gcc/ -mmcu=attiny1614 -Wall -Os -ggdb3
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -Wall -Wextra
LFLAGS = $(CFLAGS) -Wl,-Map=main.map -Wl,--defsym -Wl,__TEXT_REGION_ORIGIN__=0x0800
LIBRARIES =

# Targets
all: main.elf
clean:
	-rm -f $(OBJECTS) *.elf *.bin *.map *.hex *.syx
main.hex: main.elf
	$(OBJCOPY) -O ihex main.elf main.hex
main.bin: main.elf
	$(OBJCOPY) -O binary main.elf main.bin
main.syx: main.bin
	$(BIN2SYX) main.bin main.syx
main.elf: $(OBJECTS)
	$(LD) $(LFLAGS) -o main.elf $(OBJECTS) $(LIBRARIES)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
flash: main.syx
	amidi -s main.syx
flash_updi: main.elf
	avrdude -c jtag2updi -p t1614 -P /dev/ttyACM0 -U flash:w:main.elf
