#include <FastLED.h>
#include <math.h>
#include <esp_now.h>
#include <WiFi.h>


#define MAX_BRIGHTNESS 255
#define FIRING_THRESHOLD 10

CRGB fairy_leds[200];
CRGB tube_leds[50];

CRGB other_led_tubes[2][50];

int HUE[4];

bool is_firing = false;
int fairy_firing_idx = 0;
int tube_firing_idx = 0;
int firing_delayer = 0;
int firing_threshold = 2;


void setup() {
  FastLED.addLeds<NEOPIXEL, 14>(tube_leds, 50);
  FastLED.addLeds<WS2811, 32>(fairy_leds, 200);
  // FastLED.addLeds<NEOPIXEL, 26>(other_led_tubes[0], 50);
  // FastLED.addLeds<NEOPIXEL, 27>(other_led_tubes[1], 50);

}

float delay_ms = 0.0;
int hue_min = 0;
int hue_max = 0;
void loop() {
  hue_min = random8();
  hue_max = hue_min + 20;
  HUE[0] = random(hue_min, hue_max) % 256;
  HUE[1] = random(hue_min, hue_max) % 256;
  HUE[2] = random8();
  HUE[3] = random8();
  for (int i = 50; i < MAX_BRIGHTNESS; i++ ) {
    for (int j = 0; j < 200; j++) {
      fairy_leds[j] = CHSV(HUE[0], 255, i);
    }
    // for (int j = 0; j < 50; j++) {
    //   tube_leds[j] = CHSV(HUE[0], 255, i);
    //   // other_led_tubes[0][j] = CHSV(HUE[2], 255, i);
    //   // other_led_tubes[1][j] = CHSV(HUE[3], 255, i);
    // }
    process_synapses();
    FastLED.show();
    delay(delay_ms);
  }
  for (int i = MAX_BRIGHTNESS - 1; i >= 50; i-- ) {
    for (int j = 0; j < 200; j++) {
      fairy_leds[j] = CHSV(HUE[0], 255, i);
    }
    // for (int j = 0; j < 50; j++) {
    //   tube_leds[j] = CHSV(HUE[0], 255, i);
    //   other_led_tubes[0][j] = CHSV(HUE[2], 255, i);
    //   other_led_tubes[1][j] = CHSV(HUE[3], 255, i);
    // }
    process_synapses();
    FastLED.show();
    delay(delay_ms);
  }
}


void process_synapses() {
  if (is_firing) {
    fairy_leds[fairy_firing_idx] = CHSV(HUE[0], 255, 255);
    tube_leds[tube_firing_idx] = CHSV(HUE[1], 255, 255);
    other_led_tubes[0][tube_firing_idx] = CHSV(HUE[2], 255, 255);
    other_led_tubes[1][tube_firing_idx] = CHSV(HUE[3], 255, 255);
    if (++firing_delayer == firing_threshold) {
      firing_delayer = 0;
      if (++fairy_firing_idx >= 200) {
        is_firing = false;
      } else {
        if (fairy_firing_idx % 4 == 0) {
          tube_firing_idx++; 
        }
      }
    }
  } else if (random8() < FIRING_THRESHOLD && random8() < FIRING_THRESHOLD) {
    is_firing = true;
    fairy_firing_idx = 0;
    tube_firing_idx = 0;
  }
}
