/*
   === Tree Receiver board ===
     location: in the power box at the base of the tree
     responsibilities:
        - send patterns to all LEDs
        - receive wifi messages from player boards to trigger light patterns
        - maybe communicate with sound board?

*/
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include "interboard_comms.h"


// ******************************************************************
// --------------------------- COMMS --------------------------------
// ******************************************************************
void init_comms() {
  init_wifi();
  init_i2c();
}

void init_wifi() {
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //esp_wifi_set_mac(WIFI_IF_STA, &led_board_address[0]);
  WiFi.disconnect();

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback
  esp_now_register_recv_cb(message_received);
}

#define I2C_ADDRESS 2
void init_i2c() {
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveI2CEvent);
}

void receiveI2CEvent(int howMany) {
  while (Wire.available() > 0) {
    Serial.println(Wire.read());  // TODO: Eventually this should send wifi messages to the other boards
  }
}
#define LED_BOARD_ADDRESS 1
void sendI2C(Message msg) {
  Serial.println("Sending I2c Message");
  Wire.beginTransmission(LED_BOARD_ADDRESS);
  Wire.write(msg.sender);
  Wire.write(msg.hue);
  Wire.endTransmission();
  Serial.println("Message sent!");
}

Message incoming_msg;
void message_received(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  Serial.println("Received wifi message!");
  if (len == 1) {
    delay(10);
    Serial.println("uhhh it's doin the thing again");
    return;
  }
  memcpy(&incoming_msg, incomingData, sizeof(incoming_msg));
  sendI2C(incoming_msg);
}

void setup() {
  Serial.begin(115200);
  init_comms();
}

void loop() {
  Wire.beginTransmission(4);
  Wire.write(0);
  Wire.write(200);
  Wire.endTransmission();
  Serial.println("Message sent!");
  delay(20000);
}
