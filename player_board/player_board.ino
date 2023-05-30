/*
   === Player Board ===
   (i need a better name than player board but i am tired)
    location: There will be two of these, one in each podium
    responsibilities:
      - drive e-ink display
      - read state of all control buttons, switches, dials, sliders, etc
      - send/receive wifi messages to/from other player board
      - send wifi messages to led_board

*/

#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>


void setup() {
  Serial.begin(115200);

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
