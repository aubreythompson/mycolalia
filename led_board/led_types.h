
#include <FastLED.h>

#define TUBE true 
#define FAIRY false
#define INCREASING true
#define DECREASING false

struct Pulse {
  CRGB color;
  int string_idx;
  int firing_idx;
  int width;
  int speed;
  bool active;
  bool increasing;
  bool is_tube;
};

struct Funky {
  int tube_idx;
  int fairy_idx;
};
