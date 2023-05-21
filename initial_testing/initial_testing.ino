#include <FastLED.h>
#include <math.h>
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t myAddress[] = {0xC8, 0xF0, 0x9E, 0xF8, 0x6A, 0x60};
uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0xF3, 0x37, 0x8C};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
bool myData;

esp_now_peer_info_t peerInfo;

#define NUM_STRINGS 8
#define NUM_LEDS 100
#define MAX_BRIGHTNESS 255
#define FIRING_THRESHOLD 10
CRGB leds[NUM_STRINGS][NUM_LEDS];
int HUE = 100;
float breath_amplitude = 50.0;
bool is_firing = false;
int firing_idx = 0;
int firing_delayer = 0;
int firing_threshold = 2;


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  is_firing = true;
  firing_idx = 0;
}

void setup() {
  FastLED.addLeds<WS2811, 14>(leds[0], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 32>(leds[1], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 33>(leds[2], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 25>(leds[3], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 26>(leds[4], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 27>(leds[5], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 12>(leds[6], NUM_LEDS);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}
void loop() {
  HUE = random8();
  float delay_ms = 10.0;
  for (int i = 50; i < MAX_BRIGHTNESS; i++ ) {
    for (int j = 0; j < NUM_LEDS; j++) {
      for (int string = 0; string < NUM_STRINGS; string++) {
        leds[string][j] = CHSV(HUE, 170, i);
      }
    }
    delay(delay_ms);
    process_synapses();
    FastLED.show();
  }
  for (int i = MAX_BRIGHTNESS - 1; i >= 50; i-- ) {
    for (int j = 0; j < NUM_LEDS; j++) {
      for (int string = 0; string < NUM_STRINGS; string++) {
        leds[string][j] = CHSV(HUE, 170, i);
      }
    }
    delay(delay_ms);
    process_synapses();
    FastLED.show();
  }
}

void process_synapses() {
  if (is_firing) {
    for (int string = 0; string < NUM_STRINGS; string++) {
      leds[string][firing_idx] = CHSV(HUE, 255, 255);
    }
    if (++firing_delayer == firing_threshold) {
      firing_delayer = 0;
      if (++firing_idx > NUM_LEDS) {
        is_firing = false;
        //        // SEND EVENT
        //        // Send message via ESP-NOW
        //        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        //
        //        if (result == ESP_OK) {
        //          Serial.println("Sent with success");
        //        }
        //        else {
        //          Serial.println("Error sending the data");
        //        }
      }
    }
  } else if (random8() < FIRING_THRESHOLD && random8() < FIRING_THRESHOLD) {
    is_firing = true;
    firing_idx = 0;
  }
}