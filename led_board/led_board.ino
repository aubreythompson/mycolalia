/*
   === LED Board ===
     location: in the power box at the base of the tree
     responsibilities:
        - send patterns to all LEDs
        - receive wifi messages from player boards to trigger light patterns
        - maybe communicate with sound board?

*/


#include <FastLED.h>
#include <math.h>
#include <esp_now.h>
#include <WiFi.h>
#include "led_types.h"

#define MAX_PULSES 100
#define TUBE_COUNT 12
#define LEDS_PER_TUBE 50
#define FAIRY_COUNT 3
#define LEDS_PER_FAIRY 200

Pulse pulses[MAX_PULSES];
CRGB tubes[TUBE_COUNT][LEDS_PER_TUBE];
CRGB fairys[FAIRY_COUNT][LEDS_PER_FAIRY];
Funky p2p_funky = { 9, 0 };
Funky tree_p1_funky = {10, 1};
Funky tree_p2_funky = {11, 2};

void setup() {
  Serial.begin(115200);

  // TUBES - Base of Tree
  FastLED.addLeds<NEOPIXEL, 2>(tubes[0], LEDS_PER_TUBE);
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

void loop() {
  process_pulses();
  FastLED.show();
}


void process_pulses() {
  Pulse *p;
  CRGB *leds;
  // Light up existing pulses
  for (int i = 0; i < MAX_PULSES; i++) {
    p = &pulses[i];
    if (p->active) {
      int string_len = p->is_tube ? LEDS_PER_TUBE : LEDS_PER_FAIRY;
      leds = p->is_tube ? &tubes[p->string_idx][0] : &fairys[p->string_idx][0];
      bool lit_any = false;
      // set all the pixels (firing_idx is the leading pixel,
      //  and we are lighting "width" number of pixels)
      for (int pixel = p->firing_idx; pixel > p->firing_idx - p->width; pixel--) {
        if (pixel >= 0 && pixel < string_len) {
          lit_any = true;
          if (p->increasing) {
            leds[pixel] = p->color;
          } else {
            leds[string_len - pixel - 1] = p->color;
          }
        }
      }
      // If we didn't light any pixels, it means our pulse isn't active anymore
      if (!lit_any) {
        p->active = false;
      } else {
        p->firing_idx += p->speed;
      }
    }
  }
}

void fire_pulse(int pulse_idx, CRGB color, bool is_tube, int idx, bool increasing, int speed, int width) {
  pulses[pulse_idx].color = color;
  pulses[pulse_idx].string_idx = idx;
  pulses[pulse_idx].firing_idx = 0;
  pulses[pulse_idx].width = width;
  pulses[pulse_idx].speed = speed;
  pulses[pulse_idx].active = true;
  pulses[pulse_idx].increasing = increasing;
  pulses[pulse_idx].is_tube = is_tube;
}
