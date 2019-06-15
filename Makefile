CC=gcc
CFLAGS = -std=c11 -Wall -Wextra -Wconversion -Werror -Isrc/
CFLAGS += -ggdb3 -O0
#CFLAGS += -O3

TARGET = birb
$(TARGET): birb.h main.c
	$(CC) $^ $(CFLAGS) -o $@

.PHONY: clean
clean:
	-rm -f $(TARGET)

.PHONY: all music
all: $(TARGET)
music: $(TARGET)
	./$(TARGET) | play -r 8000 -b 8 -t raw -e signed -

.DEFAULT_GOAL = all
