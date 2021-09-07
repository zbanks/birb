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
CFLAGS = -B /home/eric/avr-test/sandbox/gcc/ -mmcu=attiny1614 -Wall -Os
#CFLAGS += -fdata-sections -ffunction-sections
LFLAGS = $(CFLAGS)
LIBRARIES =

# Targets
all: main.hex
clean:
	-rm -f $(OBJECTS) *.elf *.bin *.map *.hex
main.hex: main.elf
	$(OBJCOPY) -O ihex main.elf main.hex
main.elf: $(OBJECTS)
	$(LD) $(LFLAGS) -o main.elf $(OBJECTS) $(LIBRARIES)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
flash: main.hex
	avrdude -c jtag2updi -p t1614 -P /dev/ttyACM0 -U flash:w:main.hex	
