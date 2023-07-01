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
#include "interboard_comms.h"

#define TUBE_COUNT 12
#define LEDS_PER_TUBE 50
#define FAIRY_COUNT 3
#define LEDS_PER_FAIRY 200

CRGB tubes[TUBE_COUNT][LEDS_PER_TUBE];
CRGB fairys[FAIRY_COUNT][LEDS_PER_FAIRY];
Funky v2v = { 9, 0 }; // vesicle-to-vesicle
Funky tv1 = { 10, 1 }; // tree-to-vesicle1
Funky tv2 = { 11, 2 }; // tree-to-vesicle2
int tb[] = [0, 1, 2]; // tree base
int tt[] = [3, 4, 5, 6, 7, 8]; // tree top

void setup() {
  Serial.begin(115200);
  init_comms();
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

// Each loop will
//  - Update the state of all effects
//  - Move each game forward one time unit
//  - Trigger/adjust sounds as needed
//  - Call FastLED.show() exactly once
void loop() {
  breathe();
  round1();
  process_pulses();

  FastLED.show();
}

// ******************************************************************
// --------------------------- COMMS --------------------------------
// ******************************************************************
void init_comms() {
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback
  esp_now_register_recv_cb(message_received);
}

Message incoming_msg;
void message_received(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Serial.println("message received!");
  memcpy(&incoming_msg, incomingData, sizeof(incoming_msg));
  switch (incoming_msg.sender) {
    case Message::V1:
      Serial.println("sending pulses for p1!");
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), TUBE, tv1.tube, INCREASING, 1, 1);
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), FAIRY, tv1.fairy, INCREASING, 4, 4);
      p1_button_pressed = true;
      break;
    case Message::V2:
      Serial.println("sending pulses for p2!");
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), TUBE, tv2.tube, INCREASING, 1, 1);
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), FAIRY, tv2.fairy, INCREASING, 4, 4);
      p2_button_pressed = true;
      break;
    default:
      break;
  }
}

// ******************************************************************
// --------------------------- PULSES -------------------------------
// ******************************************************************
#define MAX_PULSES 100
Pulse pulses[MAX_PULSES];

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

int fire_pulse(CRGB color, bool is_tube, int idx, bool increasing, int speed, int width) {
  int pulse_idx = -1;
  for (int i = 0; i < MAX_PULSES; i++) {
    if (!pulses[i].active) {
      pulse_idx = i;
      break;
    }
  }
  if (pulse_idx == -1) return pulse_idx; // fail silently if we have > MAX_PULSES going already

  pulses[pulse_idx].color = color;
  pulses[pulse_idx].string_idx = idx;
  pulses[pulse_idx].firing_idx = 0;
  pulses[pulse_idx].width = width;
  pulses[pulse_idx].speed = speed;
  pulses[pulse_idx].active = true;
  pulses[pulse_idx].increasing = increasing;
  pulses[pulse_idx].is_tube = is_tube;
  return pulse_idx;
}


// ******************************************************************
// ------------------------ BREATHING -------------------------------
// ******************************************************************
// breathing could probably be improved in a few ways:
//    - make it non-linear/more breath-like
//    - make the different hues not completely random but interdependent in some way
//    - we'll probably want to figure out a more granular speed
#define MIN_BREATH 50
#define MAX_BREATH 250
int saturation = 200;
int breathing_speed = 1;
bool inhaling = true;
int breath_brightness = MIN_BREATH;
int hues[TUBE_COUNT + FAIRY_COUNT];

void breathe() {
  for (int i = 0; i < TUBE_COUNT; i++) {
    for (int j = 0; j < LEDS_PER_TUBE; j++) {
      tubes[i][j] = CHSV(hues[i], saturation, breath_brightness);
    }
  }
  for (int i = 0; i < FAIRY_COUNT; i++) {
    for (int j = 0; j < LEDS_PER_FAIRY; j++) {
      fairys[i][j] = CHSV(hues[TUBE_COUNT+i], saturation, breath_brightness);
    }
  }
  if (inhaling) {
    breath_brightness += breathing_speed;
    if (breath_brightness >= MAX_BREATH) {
      breath_brightness = MAX_BREATH;
      inhaling = false;
    }
  } else {
    breath_brightness -= breathing_speed;
    if (breath_brightness <= MIN_BREATH) {
      breath_brightness = MIN_BREATH;
      inhaling = true;
      set_random_hues();
    }
  }
}

void set_random_hues() {
  for (int i = 0; i < TUBE_COUNT + FAIRY_COUNT; i++){
    hues[i] = random8();
  }
}

// ******************************************************************
// ------------------------- GAME -----------------------------------
// ******************************************************************
bool p1_button_pressed, p2_button_pressed; // guessing we'll need these for some game state?

// Move through these states as we progress through this round
enum R1State {INACTIVE, CLUEV1, V1WRONG, V1RIGHT, CLUEV2, V2WRONG, V2RIGHT};
R1State r1_state = R1State::INACTIVE;
int firing_speed = 1;
int pulse_width = 2;
int hueToMatch;
int pulse_idx;
int firing_period_ms = 500;
uint32_t last_fired_at;
bool v1s_turn = true;
bool button_pressed = false;
int hue_guessed;
#define CORRECT_MAX_DISTANCE 5

// ROUND 1 - MATCH THE COLOR ONE AT A TIME
void round1() {
  if (r1_state == INACTIVE) {
    hueToMatch = random8();
    r1_state = R1State::CLUEV1;
    v1s_turn = true;
  }
  // Black out the one we are pulsing down
  if (v1s_turn) {
    clear_funky(tv1);
  } else {
    clear_funky(tv2);
  }
  int tube_idx = v1s_turn ? tv1.tube : tv2.tube;
  int fairy_idx = v1s_turn ? tv1.fairy : tv2.fairy;
  unsigned long now = millis();
  if (now - last_fired_at > firing_period_ms) {
    fire_pulse(CHSV(hueToMatch, 170, breath_brightness), TUBE, tube_idx, INCREASING, firing_speed, pulse_width);
    fire_pulse(CHSV(hueToMatch, 170, breath_brightness), FAIRY, fairy_idx, INCREASING, firing_speed * 4, pulse_width*4);
  }
  
}

void r1_button_pressed(int hue, int sender) {
  if (v1s_turn == (sender == Message::Sender::V1)) {
    hue_guessed = hue;
    button_pressed = true;
  }
}

void clear_funky(Funky f) {
  clear_tube(f.tube);
  clear_fairy(f.fairy);
}
void clear_tube(int idx) {
  for (int i = 0; i < LEDS_PER_TUBE; i ++) {
    tubes[idx][i] = CRGB::Black;
  }
}
void clear_fairy(int idx) {
  for (int i = 0; i < LEDS_PER_FAIRY; i ++) {
    fairys[idx][i] = CRGB::Black;
  }
}