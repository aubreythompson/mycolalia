/*
   === Vesicle Board 1 ===
    location: There will be two of these, one in each podium
    responsibilities:
      - read state of buttons and potentiometers
      - send/receive wifi messages to/from other player board
      - send wifi messages to led_board

*/


#include <esp_now.h>
#include <WiFi.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include "interboard_comms.h"
#define NUM_BUTTON_LEDS 6
#define CORRECT_MAX_DISTANCE 5


const int inputPin = 32;
const int ledPin = 33;
const int knobPin = 34;

int potSmooth;
int buttonHue;
int buttonState = 0;

//shared variables between board
int TV_hue;           //tentacle to vesicle value - get from tree board
int V2V_hue;          //vesicle to vesicle value
int roundNumber = 0;  //not sure which board to keep this on - maybe it goes on the tree board

struct Player {
  CRGB leds[6];
  int inputPin;
  int ledPin;
  int knobPin;
};

Player player = {
  { CHSV(42, 255, 255), CHSV(84, 255, 255), CHSV(126, 255, 255), CHSV(168, 255, 255), CHSV(210, 255, 255), CHSV(252, 255, 255) },
  inputPin,
  ledPin,
  knobPin
};



void setup() {
  Serial.begin(115200);
  init_comms();
  FastLED.addLeds<WS2811, ledPin>(player.leds, 6);
  FastLED.show();
  pinMode(player.inputPin, INPUT);
  pinMode(player.knobPin, INPUT);

  Serial.print("button set to ");
  potSmooth = analogRead(player.knobPin);
  buttonHue = map(potSmooth, 0, 4095, 0, 255);
  Serial.print(buttonHue);
  Serial.println(" ");
  for (int j = 0; j < NUM_BUTTON_LEDS; j++) {
    player.leds[j] = CHSV(buttonHue, 255, 255);
  }
  FastLED.show();
}



unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 100;         // interval at which to sample (milliseconds)

void loop() {
  //ROUND 0 - BREATHING STATE
  // while (roundNumber == 0)

  //is the motion sensor in this board?

  //if (someone walks up) {
  //roundNumber ++;
  //}

  //ROUND 1 - MATCH THE COLOR ONE AT A TIME - PLAYER 1
  while (roundNumber == 1) {
    TV_hue = random8();  // get from tree board actually
    Serial.print("TV1 set to ");
    Serial.print(TV_hue);
    buttonState = digitalRead(player.inputPin);
    while (buttonState == LOW) {
      updateHue();
    }
    if (abs(buttonHue - TV_hue) > CORRECT_MAX_DISTANCE) {
      Serial.print("player 1 wins!");
      winnerFlash();
      //send winning status to tree board
      //update round number, send status to other boards
      roundNumber++;
    }
  }
  //ROUND 2 - MATCH THE COLOR ONE AT A TIME - PLAYER 2
  while (roundNumber == 2) {
    //do nothing
  }

  // ROUND 3 - MATCH COLORS AT THE SAME TIME
  while (roundNumber == 3) {
    TV_hue = random8();  // get from tree board actually
    buttonState = digitalRead(player.inputPin);
    while (buttonState == LOW) {
      updateHue();
    }
    broadcast_button_pressed(buttonHue);
    if (abs(buttonHue - TV_hue) > CORRECT_MAX_DISTANCE) {
      // send correct signal to tree board
      // send correct signal to other vesicle board
      // check if other vesicle board is sending correct signal
      winnerFlash();
      //update round number, send status to other boards
      roundNumber++;
    }
  }

  //ROUND 4 - send a color to the other vesicle
  //while (roundNumber == 4)
}

void updateHue() {
  // sample potentiometer reading every 100 ms and smooth exponentially
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    potSmooth += (analogRead(player.knobPin) - potSmooth) / 4;
    buttonHue = map(potSmooth, 0, 4095, 0, 255);
    //Serial.println(buttonHue);
    for (int j = 0; j < NUM_BUTTON_LEDS; j++) {
      player.leds[j] = CHSV(buttonHue, 255, 255);
    }
    FastLED.show();
  }
}

void winnerFlash() {
  //flash 5 times
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < NUM_BUTTON_LEDS; j++) {
      player.leds[j] = CHSV(buttonHue, 255, 255);
    }
    FastLED.show();
    delay(300);
    for (int j = 0; j < NUM_BUTTON_LEDS; j++) {
      player.leds[j] = CRGB::Black;
    }
    FastLED.show();
    delay(300);
  }
}

// ******************************************************************
// --------------------------- COMMS --------------------------------
// ******************************************************************
Message outbound_msg, incoming_msg;

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

void message_received(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming_msg, incomingData, sizeof(incoming_msg));
  switch (incoming_msg.sender) {
    case Message::P2:
      // any inter-vessicle comms should go here if we want those?
      break;
    case Message::Tree:
      // tree can tell the vessicle to set things like the button color, podium color, maybe trigger effects?
      break;
    default:
      break;
  }
}

void broadcast() {
  // Broadcast a message to every device in range
  uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_add_peer(&peerInfo);
  }
  // Send message
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outbound_msg, sizeof(outbound_msg));
}

void broadcast_button_pressed(int hue) {
  outbound_msg.event = Message::BUTTON_PRESSED;
  outbound_msg.hue = hue;
  outbound_msg.sender = Message::P1;
  outbound_msg.time_sent = millis();
  broadcast();
}
