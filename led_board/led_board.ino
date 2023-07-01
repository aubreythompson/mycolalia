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
Funky p2p_funky = { 9, 0 };
Funky tree_p1_funky = { 10, 1 };
Funky tree_p2_funky = { 11, 2 };
bool p1_button_pressed, p2_button_pressed; // guessing we'll need these for some game state

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

void loop() {
  for (int i = 0; i < TUBE_COUNT; i++) {
    for (int j = 0; j < LEDS_PER_TUBE; j++) {
      tubes[i][j] = CRGB::Black;
    }
  }
  for (int i = 0; i < FAIRY_COUNT; i++) {
    for (int j = 0; j < LEDS_PER_FAIRY; j++) {
      fairys[i][j] = CRGB::Black;
    }
  }
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
    case Message::P1:
      Serial.println("sending pulses for p1!");
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), TUBE, tree_p1_funky.tube, INCREASING, 1, 1);
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), FAIRY, tree_p1_funky.fairy, INCREASING, 4, 4);
      p1_button_pressed = true;
      break;
    case Message::P2:
      Serial.println("sending pulses for p2!");
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), TUBE, tree_p2_funky.tube, INCREASING, 1, 1);
      fire_pulse(CHSV(incoming_msg.hue, 200, 255), FAIRY, tree_p2_funky.fairy, INCREASING, 4, 4);
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

void fire_pulse(CRGB color, bool is_tube, int idx, bool increasing, int speed, int width) {
  int pulse_idx = -1;
  for (int i = 0; i < MAX_PULSES; i++) {
    if (!pulses[i].active) {
      pulse_idx = i;
      break;
    }
  }
  if (pulse_idx == -1) return; // fail silently if we have > MAX_PULSES going already

  pulses[pulse_idx].color = color;
  pulses[pulse_idx].string_idx = idx;
  pulses[pulse_idx].firing_idx = 0;
  pulses[pulse_idx].width = width;
  pulses[pulse_idx].speed = speed;
  pulses[pulse_idx].active = true;
  pulses[pulse_idx].increasing = increasing;
  pulses[pulse_idx].is_tube = is_tube;
}


// ******************************************************************
// ------------------------ BREATHING -------------------------------
// ******************************************************************
