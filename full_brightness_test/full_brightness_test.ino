#include <FastLED.h>
#include <math.h>

#define TUBE_COUNT 12
#define LEDS_PER_TUBE 50
#define FAIRY_COUNT 3
#define LEDS_PER_FAIRY 200

CRGB tubes[TUBE_COUNT][LEDS_PER_TUBE];
CRGB fairys[FAIRY_COUNT][LEDS_PER_FAIRY];

void setup() {
  init_leds();
  for (int i = 0; i < TUBE_COUNT; i ++) {
    for (int j = 0; j < LEDS_PER_TUBE; j++) {
      tubes[i][j] = CRGB::White;
    }
  }
  for (int i = 0; i < FAIRY_COUNT; i ++) {
    for (int j = 0; j < LEDS_PER_FAIRY; j++) {
      fairys[i][j] = CRGB::White;
    }
  }
  FastLED.show();
}

void loop() {
  delay(1000);
  FastLED.show();
}

void init_leds() {
  // TUBES - Base of Tree
  FastLED.addLeds<NEOPIXEL, 0>(tubes[0], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 4>(tubes[1], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 5>(tubes[2], LEDS_PER_TUBE);

  // TUBES - Top of Tree
  FastLED.addLeds<NEOPIXEL, 12>(tubes[3], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 13>(tubes[4], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 14>(tubes[5], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 15>(tubes[6], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 16>(tubes[7], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 18>(tubes[8], LEDS_PER_TUBE);

  // FUNKY STRINGS
  FastLED.addLeds<NEOPIXEL, 19>(tubes[9], LEDS_PER_TUBE);
  FastLED.addLeds<WS2811, 26>(fairys[0], LEDS_PER_FAIRY);
  FastLED.addLeds<NEOPIXEL, 23>(tubes[10], LEDS_PER_TUBE);
  FastLED.addLeds<WS2811, 27>(fairys[1], LEDS_PER_FAIRY);
  FastLED.addLeds<NEOPIXEL, 25>(tubes[11], LEDS_PER_TUBE);
  FastLED.addLeds<WS2811, 32>(fairys[2], LEDS_PER_FAIRY);
}
