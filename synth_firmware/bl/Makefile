# Toolchain
TOOLCHAIN=/home/eric/avr-test/sandbox/avr8-gnu-toolchain-linux_x86_64/bin/
ARCH_PREFIX=avr-
CC=$(TOOLCHAIN)$(ARCH_PREFIX)gcc
LD=$(TOOLCHAIN)$(ARCH_PREFIX)gcc
OBJCOPY=$(TOOLCHAIN)$(ARCH_PREFIX)objcopy

# Files
C_SRC  = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(C_SRC))

# Assembler, compiler, and linker flags
CFLAGS = -B /home/eric/avr-test/sandbox/gcc/ -mmcu=attiny1614 -Wall -Os -ggdb3
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -Wall -Wextra -Werror
LFLAGS = $(CFLAGS) -Wl,-Map=bl.map
LFLAGS += -Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x1000
LIBRARIES =

# Targets
all: bl.elf
clean:
	-rm -f $(OBJECTS) *.elf *.bin *.map *.hex
bl.hex: bl.elf
	$(OBJCOPY) -O ihex bl.elf bl.hex
bl.bin: bl.elf
	$(OBJCOPY) -O binary -R .fuse bl.elf bl.bin
bl.elf: $(OBJECTS)
	$(LD) $(LFLAGS) -o bl.elf $(OBJECTS) $(LIBRARIES)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
flash: bl.elf
	avrdude -c jtag2updi -p t1614 -P /dev/ttyACM0 -U fuse8:w:bl.elf -U flash:w:bl.elf
