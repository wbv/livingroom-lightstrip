// Authors: Kali Regenold, Walter Vaughan
//    Date: August 2020

// Arduino code to run my LED strip.
// Reads in a color palette (1-6 different colors) and displays it evenly along
// the entire strip.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// dimensions and structs describing the RGB strip
#define NUM_PIX 300
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIX, PIN, NEO_GRB + NEO_KHZ800);

// a variable-sized array of the colors currently used in our palette
#define PALETTE_MAX_SZ (6)
uint32_t palette[PALETTE_MAX_SZ] = {0};
size_t   palette_sz = 0;

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(100);
  set_off();
}

void loop() {
  if(Serial.available() > 0)
  {
    update_palette();
    int pix_per_color = NUM_PIX / palette_sz;
    for(int i = 0; i < palette_sz; i++)
    {
      for(int j = 0; j < pix_per_color; j++)
      {
        strip.setPixelColor(pix_per_color*i + j, strip.gamma32(palette[i]));
      }
    }
    strip.show();
  }
  delay(25);
}

// updates the palette with the HSV colors sent over serial. First byte tells
// the size of the palette being sent, and the rest of the bytes are the colors.
// Each color is three bytes, HSV encoded. A palette larger than
// (PALETTE_MAX_SZ) simply truncates extra colors recieved.
void update_palette()
{
  uint16_t hsv[3];
  int new_palette_sz = Serial.read();

  if (new_palette_sz > PALETTE_MAX_SZ)
    new_palette_sz = PALETTE_MAX_SZ;

  // only process if a valid palette size is received
  if (new_palette_sz > 0)
  {
    palette_sz = new_palette_sz;
    for (int i = 0; i < palette_sz; i++)
    {
      hsv[0] = Serial.read(); // H
      hsv[1] = Serial.read(); // S
      hsv[2] = Serial.read(); // V

      // ColorHSV expects Hue to be a 16-bit integer, so we scale it up
      hsv[0] = (uint16_t) map(hsv[0], 0, 255, 0, 65535);

      // save the resulting color into the palette
      palette[i] = strip.ColorHSV(hsv[0], hsv[1], hsv[2]);
    }
  }

  // discard any remaining rx'd bytes
  while (Serial.available() > 0)
    Serial.read();

  return;
}


// Set `num` number of pixels to red
// Used to visually display how many bytes are available on the serial port
void set_num(int num) {
  set_off();
  for(int i = 0; i < num; i++){
    strip.setPixelColor(200+i, strip.Color(255,0,0));
  }
  strip.show();
}

// Set all pixels to off.
void set_off() {
  for(int i = 0; i < NUM_PIX; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

///////////////////////////////////////
// Set given pixel to function color
///////////////////////////////////////
void set_red(int i) {
  strip.setPixelColor(i, strip.Color(255,0,0));
  strip.show();
}

void set_gre(int i) {
  strip.setPixelColor(i, strip.Color(0,255,0));
  strip.show();
}

void set_blu(int i) {
  strip.setPixelColor(i, strip.Color(0,0,255));
  strip.show();
}
