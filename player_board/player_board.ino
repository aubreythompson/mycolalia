/*
 * === Player Board ===
 * (i need a better name than player board but i am tired)
 *  location: There will be two of these, one in each podium
 *  responsibilities:
 *    - drive e-ink display
 *    - read state of all control buttons, switches, dials, sliders, etc
 *    - send/receive wifi messages to/from other player board
 *    - send wifi messages to led_board
 *    
 */

#include <esp_now.h>
#include <WiFi.h>
