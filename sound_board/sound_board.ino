/*
 * === Sound Board ===
 *    location: in the main control box at the base of the tree
 *    responsibilities:
 *      - send control commands to the tsunami audio board
 *      - receive wifi messages from player boards (or just from led board?)
 *    resources:
 *      - https://github.com/robertsonics/Tsunami-Arduino-Serial-Library
 *      - 
 */

#include <esp_now.h>
#include <WiFi.h>

 
