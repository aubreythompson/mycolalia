/*
   === LED Board ===
     location: in the power box at the base of the tree
     responsibilities:
        - send patterns to all LEDs
        - receive wifi messages from player boards to trigger light patterns
        - maybe communicate with sound board?

*/
// #define FASTLED_ESP32_I2S
#include <FastLED.h>
#include <math.h>
#include "Tsunami.h"
#include "led_types.h"
#include "interboard_comms.h"
#include "sounds.h"

#define TUBE_COUNT 12
#define LEDS_PER_TUBE 50
#define FAIRY_COUNT 3
#define LEDS_PER_FAIRY 200

CRGB tubes[TUBE_COUNT][LEDS_PER_TUBE];
CRGB fairys[FAIRY_COUNT][LEDS_PER_FAIRY];

// Note, these are indices, not pin numbers
Funky v2v = { 9, 0 };             // vesicle-to-vesicle
Funky tv1 = { 10, 1 };            // tree-to-vesicle1
Funky tv2 = { 11, 2 };            // tree-to-vesicle2
int tb[] = { 0, 1, 2 };           // tree base
int tt[] = { 3, 4, 5, 6, 7, 8 };  // tree top
int default_firing_speed = 1;
int default_pulse_width = 1;

// ******************************************************************
// ---------------------- GAME STATE VARIABLES ----------------------
// ******************************************************************

float excitation = 1;     //goes up with all interaction
float contrarianism = 1;  //goes up with "wrong" interaction
float cooperatiion = 1;   //goes up with "right" interaction
enum dominantColor { RED,
                     YELLOW,
                     GREEN,
                     BLUE,
                     PURPLE };


/*
Possible color bands:
Red: 230 - 22 (48) fire
orange/yellow: 22 - 75 (53) air
Green: 75 - 128 (53) earth
Blue: 128 - 184 (56) water
Purple/pink: 184 - 230 (46) space
see https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors rainbow spectrum for color placements
*/

void init_leds() {
  // TUBES - Base of Tree
  FastLED.addLeds<NEOPIXEL, 22>(tubes[0], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 23>(tubes[1], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 24>(tubes[2], LEDS_PER_TUBE);

  // TUBES - Top of Tree
  FastLED.addLeds<NEOPIXEL, 25>(tubes[3], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 26>(tubes[4], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 27>(tubes[5], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 28>(tubes[6], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 29>(tubes[7], LEDS_PER_TUBE);
  FastLED.addLeds<NEOPIXEL, 30>(tubes[8], LEDS_PER_TUBE);

  // FUNKY STRINGS
  FastLED.addLeds<NEOPIXEL, 31>(tubes[9], LEDS_PER_TUBE);
  FastLED.addLeds<WS2811, 34>(fairys[0], LEDS_PER_FAIRY);
  FastLED.addLeds<NEOPIXEL, 32>(tubes[10], LEDS_PER_TUBE);
  FastLED.addLeds<WS2811, 35>(fairys[1], LEDS_PER_FAIRY);
  FastLED.addLeds<NEOPIXEL, 33>(tubes[11], LEDS_PER_TUBE);
  FastLED.addLeds<WS2811, 36>(fairys[2], LEDS_PER_FAIRY);
}

// ******************************************************************
// --------------------------- COMMS --------------------------------
// ******************************************************************
#define I2C_ADDRESS 1

void checkSerial() {
  if (Serial2.available() > 0) {
    byte b = Serial2.read();
    byte hue = Serial2.read();
    Serial.print("Got some bytes!: ");
    Serial.print(b);
    Serial.print("  ");
    Serial.println(hue);
    excitation += 0.1;
    int track;
    switch (b) {
      case Message::V1:
        fire_pulse(CHSV(hue, 170, 255), tv1, DECREASING, default_firing_speed, default_pulse_width);
        fire_pulse(CHSV(hue, 170, 255), v2v, INCREASING, default_firing_speed, default_pulse_width);
        game_button_pressed(hue, Message::V1);   
        // Play a random chime for now
        track = random8(2,12);     
        loop_track(track, false);
        play_track(track);
        break;
      case Message::V2:
        fire_pulse(CHSV(hue, 170, 255), tv2, DECREASING, default_firing_speed, default_pulse_width);
        fire_pulse(CHSV(hue, 170, 255), v2v, DECREASING, default_firing_speed, default_pulse_width);
        game_button_pressed(hue, Message::V2);
        // Play a random chime for now
        track = random8(2, 12);
        loop_track(track, false);
        play_track(track);
        break;
      default:
        break;
    }
  }
}

void init_comms() {
  Serial2.begin(9600);
}

// ******************************************************************
// --------------------------- SOUND --------------------------------
// ******************************************************************
Tsunami tsunami;

void init_sound() {
  Serial.println("initializing Tsunami!");
  tsunami.start();
  Serial.println("Tsunami initialized...");
  delay(15);
  tsunami.stopAllTracks();
  tsunami.samplerateOffset(0, 0);
  tsunami.setReporting(false);
  tsunami.masterGain(0, 0);  // Reset the master gain to 0dB
  delay(10);
}

void play_track(int track) {
  Serial.print("Playing track ");
  Serial.println(track);
  tsunami.trackPlayPoly(track, 0, false);
  tsunami.update();
}

void loop_track(int track, bool looping) {
  tsunami.trackLoop(track, looping);
  tsunami.update();
}

void stop_all_tracks() {
  tsunami.stopAllTracks();
  tsunami.update();
}

// ******************************************************************
// --------------------------- PULSES -------------------------------
// ******************************************************************
// Pulse todo:
//    - figure out a more granular way of doing speed? 1 px/frame is too fast sometimes! maybe?
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
  if (pulse_idx == -1) return pulse_idx;  // fail silently if we have > MAX_PULSES going already

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
void fire_pulse(CRGB color, Funky funky, bool increasing, int speed, int width) {
  fire_pulse(color, TUBE, funky.tube, increasing, speed, width);
  fire_pulse(color, FAIRY, funky.fairy, increasing, speed * 4, width * 4);
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
int breathing_speed = 2;
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
      fairys[i][j] = CHSV(hues[TUBE_COUNT + i], saturation, breath_brightness);
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
  for (int i = 0; i < TUBE_COUNT + FAIRY_COUNT; i++) {
    hues[i] = random8();
  }
}

// ******************************************************************
// ------------------------- GAME -----------------------------------
// ******************************************************************
bool p1_button_pressed, p2_button_pressed;  // guessing we'll need these for some game state?

// Move through these states as we progress through this round
enum R1Phase { INIT,
               CLUEV1,
               V1RIGHT,
               CLUEV2,
               V2WRONG,
               V2RIGHT,
               CLUEBOTH };
R1Phase r1_phase = R1Phase::INIT;
int firing_speed = 1;
int pulse_width = 2;
int hue_to_match;
int pulse_idx;
int firing_period_ms = 1000;  // TODO: make this a function of excitation.
uint32_t last_fired_at = 0;
uint32_t right_animation_started_at = 0;
int right_animation_duration = 5000;
int p1_hue_guessed, p2_hue_guessed;
#define CORRECT_MAX_DISTANCE 5

// ROUND 1 - MATCH THE COLOR ONE AT A TIME
void round1() {
  unsigned long now = millis();
  switch (r1_phase) {
    case R1Phase::INIT:
      // pick color
      // move to cluev1
      hue_to_match = random8();
      r1_phase = R1Phase::CLUEV1;
      firing_period_ms = 1000;
      break;
    case R1Phase::CLUEV1:
      // clear_funky(tv1);
      // clear_funky(tv2);
      // clear_funky(v2v);
      // continually send pulses of hue_to_match to V1
      if (now - last_fired_at > firing_period_ms) {
        fire_pulse(CHSV(hue_to_match, 170, 255), tv2, INCREASING, firing_speed, pulse_width);
        last_fired_at = now;
      }
      // Check button press, if pressed, check answer
      if (p1_button_pressed) {
        // If right, move to V1RIGHT
        if (abs(p1_hue_guessed - hue_to_match) < CORRECT_MAX_DISTANCE) {
          r1_phase = R1Phase::V1RIGHT;
          right_animation_started_at = now;
        }
        // reset button_pressed_state
        p1_button_pressed = false;
      }
      break;
    case R1Phase::V1RIGHT:
      // Send wider, faster, more frequent pulses for 5 seconds
      // When effect is complete, move to CLUEV2
      if (now - last_fired_at > 300) {
        fire_pulse(CHSV(hue_to_match, 170, 255), tv1, DECREASING, 3, 4);
        fire_pulse(CHSV(hue_to_match, 170, 255), v2v, INCREASING, 3, 4);
        last_fired_at = now;
      }
      if (right_animation_started_at + right_animation_duration > now) {
        hue_to_match = random8();
        r1_phase = R1Phase::CLUEV2;
      }
      break;
    case R1Phase::CLUEV2:
      clear_funky(tv1);
      clear_funky(tv2);
      clear_funky(v2v);
      // continually send pulses of hue_to_match to V2
      if (now - last_fired_at > firing_period_ms) {
        fire_pulse(CHSV(hue_to_match, 170, 255), tv2, INCREASING, firing_speed, pulse_width);
        last_fired_at = now;
      }
      // Check button press, if pressed, check answer
      if (p2_button_pressed) {
        // If right, move to V2Right
        if (abs(p2_hue_guessed - hue_to_match) < CORRECT_MAX_DISTANCE) {
          r1_phase = R1Phase::V2RIGHT;
          right_animation_started_at = now;
        }
        // reset button_pressed_state
        p2_button_pressed = false;
      }
      break;
    case R1Phase::V2RIGHT:
      // Go fucking crazy man, like really bonkers cuz they both got it right
      if (now - last_fired_at > 100) {
        fire_pulse(CHSV(random(hue_to_match - 20, hue_to_match + 20), 170, 255), tv1, DECREASING, random(1, 4), random(1, 5));
        fire_pulse(CHSV(random(hue_to_match - 20, hue_to_match + 20), 170, 255), v2v, INCREASING, random(1, 4), random(1, 5));
        fire_pulse(CHSV(random(hue_to_match - 20, hue_to_match + 20), 170, 255), v2v, DECREASING, random(1, 4), random(1, 5));
        fire_pulse(CHSV(random(hue_to_match - 20, hue_to_match + 20), 170, 255), tv2, DECREASING, random(1, 4), random(1, 5));
        last_fired_at = now;
      }
      if (right_animation_started_at + right_animation_duration > now) {
        r1_phase = R1Phase::CLUEBOTH;
        hue_to_match = random8();
      }
      break;
    case R1Phase::CLUEBOTH:
      break;
    default:
      break;
  }
}

void game_button_pressed(int hue, int sender) {
  // TODO: Add game state for things like "which round are we in"
  if (sender == Message::V1) {
    p1_button_pressed = true;
    p1_hue_guessed = hue;
  } else {
    p2_button_pressed = true;
    p2_hue_guessed = hue;
  }
}

void clear_funky(Funky f) {
  clear_tube(f.tube);
  clear_fairy(f.fairy);
}
void clear_tube(int idx) {
  for (int i = 0; i < LEDS_PER_TUBE; i++) {
    tubes[idx][i] = CRGB::Black;
  }
}
void clear_fairy(int idx) {
  for (int i = 0; i < LEDS_PER_FAIRY; i++) {
    fairys[idx][i] = CRGB::Black;
  }
}


// void test_tsunami_forever() {
//   delay(1000);
//   Serial.begin(115200);
//   Serial.println("Starting tsunami in 2...");
//   delay(2000);
//   tsunami.start();
//   Serial.println("Tsunami started");
//   Serial.println("Sending StopAll command in 2...");
//   delay(2000);
//   tsunami.stopAllTracks();
//   Serial.println("Calling update on tsunami in 2...");
//   delay(2000);
//   tsunami.update();
//   while (true) {
//     Serial.println("Playing track 5 in 2...");
//     delay(2000);
//     tsunami.trackPlayPoly(5, 0, true);
//     tsunami.update();
//   }
// }

// void test_tsunami_raw_serial() {
//   Serial.println("Setting up serial2");
//   Serial2.begin(57600, SERIAL_8N1, 33, 17);
//   Serial.println("Serial2 setup success");
//   uint8_t txbuf[10];
//   uint8_t o;

//   o = 1 & 0x07;
//   txbuf[0] = 0xf0;
//   txbuf[1] = 0xaa;
//   txbuf[2] = 0x0a;
//   txbuf[3] = 3;
//   txbuf[4] = (uint8_t)1;
//   txbuf[5] = (uint8_t)1;
//   txbuf[6] = (uint8_t)(1 >> 8);
//   txbuf[7] = (uint8_t)o;
//   txbuf[8] = (uint8_t)0;
//   txbuf[9] = 0x55;
//   Serial.println("Sending txbuf");
//   delay(2000);
//   Serial2.write(txbuf, 10);
//   Serial.println("txbuf sent");
// }

void setup() {
  Serial.begin(9600);
  init_comms();
  init_leds();
  init_sound();
  // loop_track(BREATH, true);
  // play_track(BREATH);
}

unsigned long last_played_at = 0;
// Each loop will
//  - Update the state of all effects
//  - Move each game forward one time unit
//  - Trigger/adjust sounds as needed
//  - Call FastLED.show() exactly once
void loop() {
  checkSerial();
  breathe();
  //round1();
  process_pulses();
  FastLED.show();
  // unsigned long now = millis();
  // if (random8() > 252) {
  //   fire_pulse(CHSV(random8(), 170, 255), TUBE, random8(TUBE_COUNT), INCREASING, random8(1, 3), random8(1, 5));
  //   int track = random(2, 12);
  //   loop_track(track, false);
  //   play_track(track);
  // }
}
