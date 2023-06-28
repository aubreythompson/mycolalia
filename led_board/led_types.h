
#include <FastLED.h>

#define MAX_PULSES_PER_STRING 10
#define LEDS_PER_TUBE 50
#define LEDS_PER_STRING 200

struct Pulse {
  CRGB color;
  int speed; // This will be _some_ measure of speed here, unsure what type yet
  bool increasing; // is the pulse going high idx downward or low idx upward
  bool active;
};


struct Tube {
  CRGB leds[LEDS_PER_TUBE];
  Pulse pulses[MAX_PULSES_PER_STRING];
  int pin;
};

struct Fairy {
  CRGB leds[LEDS_PER_STRING];
  Pulse pulses[MAX_PULSES_PER_STRING];
  int pin;
};

struct FunkyString {
    Tube tube;
    Fairy fairy;
    Pulse pulses[MAX_PULSES_PER_STRING];
    int tube_pin;
    int fairy_pin;
};
