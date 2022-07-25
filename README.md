Programs for running the RGB LED lightstrip in our living room.
---------------------------------------------------------------


The hardware in this project includes:
- LED light strip containing 300 serially-connected SK6812 RGB LEDs
- Arduino Uno R3
- Computer (running Linux) connected via USB to the Arduino.

This repo houses software for
1. Arduino code which takes color palette input over USB/serial and controls
   the LEDs
2. Python script which generates a color palette and sends it over USB to the
   Arduino


### 1. Light Runner

The light runner is a modified version of the sample code distributed by
AdaFruit for their NeoPixel line of LED strips. This code is compatible with
both WS2812 LEDs and SK6812 LEDs. For this project, the LED count is hard-coded
to 300, but that parameter is theoretically easy to change for your own use.

The display pattern we use is an out/up-ward shifting wave: we blend each color
for a given palette linearly, and then continuously shift the pixel values
rightward in a ring buffer, where low memory corresponds to low-address LEDs
(1, 2, 3...) and high memory corresponds to the high-address LEDs (...298, 299,
300).


### 2. Send Palette

The palette generator scrapes website data for a simple, 3-to-5-color palette
which is then sent to the Arduino to be displayed. We run this script
automatically every hour just to keep colors fresh, but you could run it
however often or infrequently you'd want, provided the Arduino has had time to
accept a new palette.

