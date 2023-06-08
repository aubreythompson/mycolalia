/*

*/

#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>

#define NUM_STRINGS 2
#define NUM_PLAYERS 2
#define NUM_LEDS 200
#define MAX_BRIGHTNESS 255
#define CORRECT_MAX_DISTANCE 5
CRGB leds[NUM_STRINGS][NUM_LEDS];
int TENTACLE_HUE[NUM_STRINGS];
int BUTTON_HUE[NUM_PLAYERS];
int potentiometerHue;
float breath_amplitude = 50.0;
bool is_firing = false;
int firing_idx = 0;
int firing_delayer = 0;
int firing_threshold = 2;

//put in the pin numbers here
int button1Pin = 1;
int button2Pin = 2;
int tentacle1Pin = 3;
int tentacle2Pin = 4;

struct Button {
  CRGB leds[6];
  int inputPin;
  int ledPin;
};

for (int i = 0; i < NUM_PLAYERS; i++) {
  TENTACLE_HUE[i] = random8(256);
  BUTTON_HUE[i] = random8(256);
}

Button players[] = {
  {
    {CHSV(BUTTON_HUE[1], 255, 255), CHSV(BUTTON_HUE[1], 255, 255), CHSV(BUTTON_HUE[1], 255, 255), CHSV(BUTTON_HUE[1], 255, 255), CHSV(BUTTON_HUE[1], 255, 255), CHSV(BUTTON_HUE[1], 255, 255)},
    35,
    14
  },


Button player2 = {
    {CHSV(BUTTON_HUE[2], 255, 255), CHSV(BUTTON_HUE[2], 255, 255), CHSV(BUTTON_HUE[2], 255, 255), CHSV(BUTTON_HUE[2], 255, 255), CHSV(BUTTON_HUE[2], 200, 255), CHSV(BUTTON_HUE[2], 255, 255)},
    34,
    26
  }
};


void setup() {
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, tentacle1Pin>(leds[1], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, tentacle2Pin>(leds[2], NUM_LEDS);
  FastLED.addLeds<WS2811, button1Pin>(buttons[0].leds, 6);
  FastLED.addLeds<WS2811, button2Pin>(buttons[1].leds, 6);
  pinMode(player1.inputPin, INPUT);
  pinMode(player2.inputPin, INPUT);
  FastLED.show();

}

int buttonState = 0;
void loop() {
  for (int i = 0; i < NUM_PLAYERS; i++) {
    if (matchRound(i)) {
      break;
    }
  }

}

bool matchRound(int player) {
  int answer = getPotentiometerHue();
  buttonState = digitalRead(buttons[i].inputPin);
  //in this case it would only try to read it if the button was clicked. do we want to read all the time? how does it respond when you get in range but haven't clicked?
  if (buttonState == HIGH) {      
    if (abs(answer-TENTACLE_HUE[i] <= CORRECT_MAX_DISTANCE) {
       return true;
    }
  }
  return false;
}

int getPotentiometerHue() {
  int numClicksPerHue = 16; //4096 clicks / 256 hues
  int potentiometerPin = 34;
  int sensorValue = analogRead(potentiometerPin);
  return sensorValue/numClicksPerHue; //integer division
}

