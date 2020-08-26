
## change these to your pleasing
# this is the Arduino project name
PROJ := led_color_runner

# board package as expected by avrdude
BOARD := arduino:avr:uno

# board package dir (TODO: get this automatically from "BOARD"
BOARD_DIR := arduino.avr.uno

# device port that we'll program to
PORT := /dev/ttyACM0

PROJ_TGT := $(PROJ)/build/$(BOARD_DIR)/$(PROJ).ino.hex

all: $(PROJ_TGT)

$(PROJ_TGT): $(PROJ)/$(PROJ).ino
	arduino-cli compile -v -b $(BOARD) -p $(PORT) $(PROJ)

install: $(PROJ_TGT)
	arduino-cli upload -v -b $(BOARD) -p $(PORT) $(PROJ)

change-color:
	./led_color_runner.py

clean:
	cd $(PROJ) && rm -rf build

.PHONY: all install change-color clean
