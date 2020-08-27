// Authors: Kali Regenold, Walter Vaughan
// Date: August 2020

// Arduino code to run my LED strip.
// Reads in a color palette (1-6 different colors) and displays it evenly along
// the entire strip.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define MIN3(x, y, z) (x<y ? (x<z?x:z) : (y<z?y:z))
#define MAX3(x, y, z) (x>y ? (x>z?x:z) : (y>z?y:z))
#define NUM_PIX        (300)  // Number of pixels on LED strip
#define PIN              (6)  // Data pin to LED strip
#define PALETTE_MAX_SZ   (6)  // Max palette size (easily divides NUM_PIX)

// Struct to hold HSV values in the sizes NeoPixel expects
struct hsv_t {
  uint16_t h;
  uint8_t s;
  uint8_t v;
};

// Struct to hold byte-sized RGB values
struct rgb_t {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// LED strip object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIX, PIN, NEO_GRB + NEO_KHZ800);

// Current color palettes
hsv_t palette_hsv[PALETTE_MAX_SZ] = {0};
rgb_t palette_rgb[PALETTE_MAX_SZ] = {0};

// Size of the current palette
size_t palette_sz = 0;

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(100);
  set_off();
}

void loop() {
  // Wait for palette to be RX'd
  if(Serial.available() > 0)
  {
    // Read in palette (HSV)
    update_palette();

    int pix_per_color = NUM_PIX / palette_sz;

    // Display palette evenly across strip
    for(int i = 0; i < palette_sz; i++)
    {
      for(int j = 0; j < pix_per_color; j++)
      {
        strip.setPixelColor(pix_per_color*i + j, strip.gamma32(strip.ColorHSV(palette_hsv[i].h, palette_hsv[i].s, palette_hsv[i].v)));
      }
    }
    strip.show();
  }
  delay(25);
}

// Update the RGB and HSV palette arrays
// Read in RGB values and convert to HSV
// Truncate palette if longer than (PALETTE_MAX_SZ)
void update_palette()
{
  int new_palette_sz = Serial.read();

  // Truncate palette size
  if (new_palette_sz > PALETTE_MAX_SZ)
  {
    new_palette_sz = PALETTE_MAX_SZ;
  }

  // Only process if a valid palette size is received
  if (new_palette_sz > 0)
  {
    // Set global palette size
    palette_sz = new_palette_sz;

    // Read in RGB bytes and convert to HSV
    for (int i = 0; i < palette_sz; i++)
    {
      // Wait for whole color to be RX'd
      while(Serial.available() < 3);
      palette_rgb[i].r = Serial.read();
      palette_rgb[i].g = Serial.read();
      palette_rgb[i].b = Serial.read();

      RGB_to_HSV(palette_rgb[i], palette_hsv[i]);
    }
  }

  // Discard any remaining RX'd bytes
  while (Serial.available() > 0)
    Serial.read();

  return;
}

// Convert RGB [0..255] to HSV [0..65535], [0..255], [0..255]
// Explanation to formula: wikipedia.org/wiki/HSL_and_HSV
void RGB_to_HSV(rgb_t &rgb, hsv_t &hsv)
{
  float rgb_f[3] = {rgb.r/255.0f, rgb.g/255.0f, rgb.b/255.0f};
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
  hsv.h = (uint16_t) map_f(hsv_f[0], 0, 360, 0, 65535);
  hsv.s = (uint8_t)  map_f(hsv_f[1], 0,   1, 0,   255);
  hsv.v = (uint8_t)  map_f(hsv_f[2], 0,   1, 0,   255);
}

///////////////////////////////////////
// Auxiliary functions
///////////////////////////////////////

// Identical to Arduino's map function but with floats
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

// Set all pixels to off
void set_off() {
  for(int i = 0; i < NUM_PIX; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

// Set ith pixel to red
void set_red(int i) {
  strip.setPixelColor(i, strip.Color(255,0,0));
  strip.show();
}

// Set ith pixel to green
void set_gre(int i) {
  strip.setPixelColor(i, strip.Color(0,255,0));
  strip.show();
}

// Set ith pixel to blue
void set_blu(int i) {
  strip.setPixelColor(i, strip.Color(0,0,255));
  strip.show();
}
