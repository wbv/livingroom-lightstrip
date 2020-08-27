// Authors: Kali Regenold, Walter Vaughan
//    Date: August 2020

// Arduino code to run my LED strip.
// Reads in a color palette (1-6 different colors) and displays it evenly along
// the entire strip.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define MIN3(x, y, z) (x<y ? (x<z?x:z) : (y<z?y:z))
#define MAX3(x, y, z) (x>y ? (x>z?x:z) : (y>z?y:z))

// dimensions and structs describing the RGB strip
#define NUM_PIX 300
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIX, PIN, NEO_GRB + NEO_KHZ800);

// a variable-sized array of the colors currently used in our palette
#define PALETTE_MAX_SZ (6)
uint16_t palette_hsv[PALETTE_MAX_SZ][3] = {0};
uint8_t palette_rgb[PALETTE_MAX_SZ][3] = {0};
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
        strip.setPixelColor(pix_per_color*i + j, strip.gamma32(strip.ColorHSV(palette_hsv[i][0], palette_hsv[i][1], palette_hsv[i][2])));
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
  // uint8_t rgb[3] = {0};
  uint16_t hsv[3] = {0};
  int new_palette_sz = Serial.read();

  if (new_palette_sz > PALETTE_MAX_SZ)
    new_palette_sz = PALETTE_MAX_SZ;

  // only process if a valid palette size is received
  if (new_palette_sz > 0)
  {
    palette_sz = new_palette_sz;
    for (int i = 0; i < palette_sz; i++)
    {
      while(Serial.available() < 3);
      palette_rgb[i][0] = Serial.read();
      palette_rgb[i][1] = Serial.read();
      palette_rgb[i][2] = Serial.read();

      RGB_to_HSV(palette_rgb[i], palette_hsv[i]);
    }
  }

  // discard any remaining rx'd bytes
  while (Serial.available() > 0)
    Serial.read();

  return;
}

void RGB_to_HSV(uint8_t rgb[3], uint16_t hsv[3])
{
  float rgb_f[3] = {rgb[0]/255.0f, rgb[1]/255.0f, rgb[2]/255.0f};
  float hsv_f[3] = {0.0, 0.0, 0.0};

  float c_min = MIN3(rgb_f[0], rgb_f[1], rgb_f[2]);
  float c_max = MAX3(rgb_f[0], rgb_f[1], rgb_f[2]);
  float c_delta = c_max - c_min;

  // Hue
  if(c_delta == 0)
  {
    hsv_f[0] = 0;
  }
  else if(c_max == rgb_f[0])
  {
    hsv_f[0] = (rgb_f[1] - rgb_f[2]) / c_delta;
  }
  else if(c_max == rgb_f[1])
  {
    hsv_f[0] = (rgb_f[2] - rgb_f[0]) / c_delta + 2;
  }
  else
  {
    hsv_f[0] = (rgb_f[0] - rgb_f[1]) / c_delta + 4;
  }
  hsv_f[0] *= 60;
  if(hsv_f[0] < 0)
  {
    hsv_f[0] += 360;
  }

  // Saturation
  if(c_max == 0.0)
  {
    hsv_f[1] = 0.0;
  }
  else
  {
    hsv_f[1] = c_delta / c_max;
  }

  // Value
  hsv_f[2] = c_max;

  // Map to NeoPixel HSV ranges
  hsv[0] = (uint16_t) map_f(hsv_f[0], 0, 360, 0, 65535);
  hsv[1] = (uint16_t) map_f(hsv_f[1], 0,   1, 0,   255);
  hsv[2] = (uint16_t) map_f(hsv_f[2], 0,   1, 0,   255);
}

float map_f(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
