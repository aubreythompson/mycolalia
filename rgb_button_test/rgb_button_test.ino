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

Button buttons[] = {
  {
    {CHSV(140, 255, 255), CHSV(140, 255, 255), CHSV(140, 255, 255), CHSV(247, 200, 255), CHSV(247, 200, 255), CHSV(247, 200, 255)},
    35,
    14
  },
  {
    {CHSV(140, 255, 255), CHSV(140, 255, 255), CHSV(140, 255, 255), CHSV(247, 200, 255), CHSV(247, 200, 255), CHSV(247, 200, 255)},
    34,
    26
  },
  {
    {CHSV(140, 255, 255), CHSV(140, 255, 255), CHSV(140, 255, 255), CHSV(247, 200, 255), CHSV(247, 200, 255), CHSV(247, 200, 255)},
    32,
    12
  }
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
  for (int i = 0; i < 3; i++) {
    buttonState = digitalRead(buttons[i].inputPin);
    if (buttonState == HIGH) {
      CRGB temp = buttons[i].leds[0];
      for (int j = 0; j < 5; j++) {
        buttons[i].leds[j] = buttons[i].leds[j + 1];
      }
      buttons[i].leds[5] = temp;
      FastLED.show();
    }
  }
  delay(50);
}
