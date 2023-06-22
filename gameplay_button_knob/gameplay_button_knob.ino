

#include <esp_now.h>
#include <WiFi.h>
#define  FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMono24pt7b.h>


#define NUM_STRINGS 2
#define NUM_PLAYERS 1
#define NUM_BUTTON_LEDS 6
#define NUM_LEDS 200
#define MAX_BRIGHTNESS 255
#define CORRECT_MAX_DISTANCE 5
//CRGB leds[NUM_STRINGS][NUM_LEDS];
int TENTACLE_HUE[NUM_STRINGS];

const int player1InputPin = 26;
const int player1LedPin =  25;
const int player1KnobPin = 34;

const int player2InputPin = 32;
const int player2LedPin = 33;
const int player2KnobPin = 34;

int potSmooth[NUM_PLAYERS];
int buttonHue[NUM_PLAYERS];
int buttonState[NUM_PLAYERS] = {0};

struct Player {
  CRGB leds[6];
  int inputPin;
  int ledPin;
  int knobPin;
};

Player players[] = {
  {
      {CHSV(42, 255, 255), CHSV(84, 255, 255), CHSV(126, 255, 255), CHSV(168, 255, 255), CHSV(210, 255, 255), CHSV(252, 255, 255)},
      player2InputPin,
      player2LedPin,
      player2KnobPin 
    },
    {
      {CHSV(42, 255, 255), CHSV(84, 255, 255), CHSV(126, 255, 255), CHSV(168, 255, 255), CHSV(210, 255, 255), CHSV(252, 255, 255)},
      player2InputPin,
      player2LedPin,
      player2KnobPin 
    }
  };


void setup() {
  
  Serial.begin(115200);
  // FastLED.addLeds<NEOPIXEL, tentacle1Pin>(leds[1], NUM_LEDS);
  // FastLED.addLeds<NEOPIXEL, tentacle2Pin>(leds[2], NUM_LEDS);
  FastLED.addLeds<WS2811, player2LedPin>(players[0].leds, 6);
 // FastLED.addLeds<WS2811, player2LedPin>(players[1].leds, 6);
  FastLED.show();
  pinMode(players[0].inputPin, INPUT);
 // pinMode(players[1].inputPin, INPUT);
  pinMode(players[0].knobPin,INPUT);
 // pinMode(players[1].knobPin,INPUT);


  for (int i = 0; i < NUM_PLAYERS; i++) {
    Serial.print("button ");
    Serial.print(i);
    Serial.print(" set to ");
    potSmooth[i] = analogRead(players[i].knobPin);
    buttonHue[i] = map(potSmooth[i], 0, 4095, 0, 255);
    Serial.print(buttonHue[i]);
    Serial.println(" ");
    TENTACLE_HUE[i] = random8();
    Serial.print("tentacle ");
    Serial.print(i);
    Serial.print(" set to ");
    Serial.print(TENTACLE_HUE[i]);
    Serial.println(" ");
    for (int j = 0; j < NUM_BUTTON_LEDS; j++) {
      players[i].leds[j] = CHSV(buttonHue[i], 255, 255);
    }
    FastLED.show();
  }
}

unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 100;  // interval at which to sample (milliseconds)

void loop() {  

  // sample potentiometer reading every 100 ms and smooth exponentially
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    for (int i = 0; i < NUM_PLAYERS; i++) {
      potSmooth[i] += (analogRead(players[i].knobPin) - potSmooth[i])/4;
      Serial.println(potSmooth[i]);
      buttonHue[i] = map(potSmooth[i], 0, 4095, 0, 255);
      Serial.println(buttonHue[i]);

      for (int j = 0; j < NUM_BUTTON_LEDS; j++) {   
        players[i].leds[j] = CHSV(buttonHue[i], 255, 255);
      }
      FastLED.show();
      delay(50);
      //ROUND 1 - MATCH THE COLOR
      buttonState[i] = digitalRead(players[i].inputPin);
      if (buttonState[i] == HIGH) {
        if (abs(buttonHue[i]-TENTACLE_HUE[i]) <= CORRECT_MAX_DISTANCE) {
          Serial.print("player ");
          Serial.print(i);
          Serial.print(" wins!");
          winnerFlash(i);
          TENTACLE_HUE[i] = random8();
          Serial.println(TENTACLE_HUE[i]);
          break;
        }
      }
    }
  }
}

// bool matchRound(int player) {
//   buttonState = digitalRead(players[player].inputPin);
//   Serial.println(buttonState[player]);
//   if (buttonState[player] == HIGH) {      
//     Serial.println("button pressed!");
//     return abs(buttonHue[player]-TENTACLE_HUE[player]) <= CORRECT_MAX_DISTANCE;
//   }
// }


void winnerFlash(int player) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < NUM_BUTTON_LEDS; j++) {   
      players[player].leds[j] = CHSV(buttonHue[player], 255, 255);
    }
    FastLED.show();
    delay(300);
    for (int j = 0; j < NUM_BUTTON_LEDS; j++) {   
      players[player].leds[j] = CRGB::Black;
    }
    FastLED.show();
    delay(300);
  }

}


