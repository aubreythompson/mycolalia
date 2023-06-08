/*

*/

#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>

struct Button {
  CRGB leds[6];
  int inputPin;
  int ledPin;
};

hue1 = random8(256);
hue2 = random8(256);

Button player1 = 
  {
    {CHSV(hue1, 255, 255), CHSV(hue1, 255, 255), CHSV(hue1, 255, 255), CHSV(hue1, 255, 255), CHSV(hue1, 255, 255), CHSV(hue1, 255, 255)},
    35,
    14
  };


Button player2 = {
    {CHSV(hue2, 255, 255), CHSV(hue2, 255, 255), CHSV(hue2, 255, 255), CHSV(hue2, 255, 255), CHSV(hue2, 200, 255), CHSV(hue2, 255, 255)},
    34,
    26
  };


void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, 14>(buttons[0].leds, 6);
  FastLED.addLeds<WS2811, 26>(buttons[1].leds, 6);
  FastLED.addLeds<WS2811, 12>(buttons[2].leds, 6);
  for (int i = 0; i < 3; i++) {
    pinMode(buttons[i].inputPin, INPUT);
  }
  FastLED.show();

}

int buttonState = 0;
void loop() {
  round1();

}

void round1() {
  
}

int getPotentiometerHue() {
  int numClicksPerHue = 16; //4096 clicks / 256 hues
  int potentiometerPin = 34;
  int sensorValue = analogRead(potentiometerPin);
  return sensorValue/numClicksPerHue; //integer division
}

//cool rainy code
  // for (int i = 0; i < 2; i++) {
  //   buttonState = digitalRead(buttons[i].inputPin); // this is reading whether the button is HIGH or LOW - whether it is being pushed
  //   if (buttonState == HIGH) { // on push
  //     CRGB temp = buttons[i].leds[0]; //good ol swap
  //     for (int j = 0; j < 5; j++) {
  //       buttons[i].leds[j] = buttons[i].leds[j + 1]; // in fact a 6 way swap
  //     }
  //     buttons[i].leds[5] = temp; // cool
  //     FastLED.show();
  //   }
  // }
  // delay(50);
