// Authors: Kali Regenold, Walter Vaughan
// Date: August 2020

// Arduino code to run a 300-pixel individually-addressable RGB LED strip.
// Reads in a color palette (1-6 different colors) and displays it evenly along
// the entire strip.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <math.h>

#define MIN3(x, y, z) (x<y ? (x<z?x:z) : (y<z?y:z))
#define MAX3(x, y, z) (x>y ? (x>z?x:z) : (y>z?y:z))
#define NUM_PIX        (300)  // Number of pixels on LED strip
#define PIN              (6)  // Data pin to LED strip
#define PALETTE_MAX_SZ   (6)  // Max palette size (easily divides NUM_PIX)

// Struct to hold byte-sized RGB values
struct rgb_t {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// LED strip object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIX, PIN, NEO_GRB + NEO_KHZ800);

// Current color palettes
//hsv_t palette_hsv[PALETTE_MAX_SZ] = {0};
rgb_t palette_rgb[PALETTE_MAX_SZ] = {0};

// Size of the current palette
size_t palette_sz = 0;
// The number of pixels between each color, a function of palette_sz
size_t interp = NUM_PIX;


void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(100);
  set_off();
}

void loop() {
  // raw pixel buffer data
  volatile uint8_t *pixels;
  uint8_t last_pixel[3];

  // Update the palette if we RX anything over Serial
  if(Serial.available() > 0)
  {
    // Read in new palette colors over serial connection
    update_palette();

    // Only generate gradients for non-empty palettes
    if (palette_sz == 0)
    {
      set_off();
    }
    else
    {
      generate_gradient();
      strip.show();
    }
  }

  // Hack: marquee through the generated gradient of pixel colors, relies on
  // knowing the pixel format of the NeoPixel strip (ours is GRB: 3 bytes)
  if (palette_sz > 0)
  {
    pixels = strip.getPixels();

    // Move the pixel buffer in a circle by three bytes, wrapping the last pixel
    last_pixel[0] = pixels[(NUM_PIX-1)*3];
    last_pixel[1] = pixels[(NUM_PIX-1)*3 + 1];
    last_pixel[2] = pixels[(NUM_PIX-1)*3 + 2];
    memmove(
      pixels + 3*sizeof(uint8_t),
      pixels,
      sizeof(uint8_t)*(NUM_PIX-1)*3
    );
    pixels[0] = last_pixel[0];
    pixels[1] = last_pixel[1];
    pixels[2] = last_pixel[2];

    strip.show();
    delay(40);
  }
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
    interp = NUM_PIX / palette_sz;

    while(Serial.available() < 3*palette_sz);

    // Read in RGB bytes and convert to HSV
    for (int i = 0; i < palette_sz; i++)
    {
      palette_rgb[i].r = strip.gamma8(Serial.read());
      palette_rgb[i].g = strip.gamma8(Serial.read());
      palette_rgb[i].b = strip.gamma8(Serial.read());
    }
  }

  // Discard any remaining RX'd bytes
  while (Serial.available() > 0)
    Serial.read();

  return;
}


// Compute the HSV value on the gradient of colors stretching the whole strip.
// `pixel` is somewhere [ 0, NUM_PIX ) and the gradient is each color in the
// palette, spread out evenly and wrapping back around to the start.
void generate_gradient(void)
{
  // Individual pixel being calculated, in HSV and NeoPixel format
  rgb_t    px;
  uint32_t px32;

  // Interpolation variables
  rgb_t *start, *stop;
  int delta_r, delta_g, delta_b;

  // Color index into the palette, and index relative to start color
  size_t color, offset;

  // Empty palettes: black
  if (palette_sz < 1)
  {
    for (int i = 0; i < NUM_PIX; i++)
    {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }

  // Singular color: totally fill gradient with the color
  else if (palette_sz == 1)
  {
    for (int i = 0; i < NUM_PIX; i++)
    {
      px.r = palette_rgb[0].r;
      px.g = palette_rgb[0].g;
      px.b = palette_rgb[0].b;
      strip.setPixelColor(i, px.r, px.g, px.b);
    }
  }

  // 2+ colors: interpolate between them.
  else
  {
    for (int i = 0; i < NUM_PIX; i++)
    {
      color = i / interp;
      offset = i % interp;

      // Interpolate colors from start to stop
      start = &palette_rgb[color];
      stop  = &palette_rgb[(color + 1 == palette_sz) ? 0 : color + 1];
      delta_r = (int)stop->r - (int)start->r;
      delta_g = (int)stop->g - (int)start->g;
      delta_b = (int)stop->b - (int)start->b;

      // Red interpolation
      px.r = start->r;
      if (delta_r != 0)
      {
        px.r += (uint8_t)floor((float)delta_r * (float)offset / (float)interp);
      }

      // Green interpolation
      px.g = start->g;
      if (delta_g != 0)
      {
        px.g += (uint8_t)floor((float)delta_g * (float)offset / (float)interp);
      }

      // Blue interpolation
      px.b = start->b;
      if (delta_b != 0)
      {
        px.b += (uint8_t)floor((float)delta_b * (float)offset / (float)interp);
      }

      strip.setPixelColor(i, px.r, px.g, px.b);
    }
  }
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
  for(int i = 0; i < num && i < NUM_PIX; i++){
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
