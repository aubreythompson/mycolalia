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
#include <math.h>

#include "interboard_comms.h"


// ******************************************************************
// --------------------------- COMMS --------------------------------
// ******************************************************************
void init_comms() {
  init_wifi();
  Serial2.begin(9600);
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

void sendSerial(Message msg) {
  Serial2.write(msg.sender);
  Serial2.write(msg.hue);
  Serial.println("Message sent!");
}

Message incoming_msg;
unsigned long last_msg_at = 0;
void message_received(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  Serial.println("Received wifi message!");
  if (len == 1) {
    delay(10);
    Serial.println("uhhh it's doin the thing again");
    return;
  }
  memcpy(&incoming_msg, incomingData, sizeof(incoming_msg));
  unsigned long now = millis();
  if (now - last_msg_at > 200) {
    sendSerial(incoming_msg);
    last_msg_at = now;
  }
}

void setup() {
  Serial.begin(115200);
  init_comms();
}

void loop() {
}
