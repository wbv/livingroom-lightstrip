
# Compiler options
##################

# which avr-gcc (compiler) to use
CCPREFIX ?= avr-
CC := $(CCPREFIX)gcc

# these flags can optionally be overridden (default: optimize for code size)
CFLAGS ?= -Os -std=c11 -Wall -Wno-main -pedantic
LDFLAGS ?= -flto -fuse-linker-plugin
#LDFLAGS += -Wl,-Map,light-runner.map

# object-copy to use (default: same prefix as your gcc)
OBJCOPY ?= $(CC:%-gcc=%-objcopy)


# Programmer options
####################

# which avrdude (programmer) to use
AVRDUDE ?= avrdude

# force-upload, skip verification
AVRDUDEFLAGS ?= -F -V
# USB serial port device where the Arduino is attached:
#  Note: this may vary wildly depending on your machine's configuration
SERIALPORT ?= /dev/ttyACM0
SERIALBAUD ?= 115200


# disable default rules
.SUFFIXES:

# default behavior is to build the light-runner firmware in its programmable format (Intel hex)
all: light-runner.hex

# build a compiled AVR object file
light-runner.o: light-runner.c usart.h
	$(CC) $(CFLAGS) -DF_CPU=16000000UL -mmcu=atmega328p -c $< -o $@

usart.o: usart.c usart.h
	$(CC) $(CFLAGS) -DF_CPU=16000000UL -mmcu=atmega328p -c $< -o $@

# build an AVR executable
light-runner.elf: light-runner.o usart.o
	$(CC) -mmcu=atmega328p $(LDFLAGS) $^ -o $@

# extract the text and data sections to form a programmable file (Intel hex format)
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# installation is programming to the arduino attached to the serial port
install: light-runner.hex
	$(AVRDUDE) $(AVRDUDEFLAGS) -c arduino -p m328p -P $(SERIALPORT) -b $(SERIALBAUD) -U flash:w:$^:i

change-color:
	./send_palette.py

clean:
	rm -rf *.hex *.elf *.o *.map

.PHONY: all install change-color clean
.SECONDARY: light-runner.elf light-runner.o usart.o
