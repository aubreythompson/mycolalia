/*  
 * === LED Board ===
 *   location: in the power box at the base of the tree
 *   responsibilities:
 *      - send patterns to all LEDs
 *      - receive wifi messages from player boards to trigger light patterns
 *      - maybe communicate with sound board? 
 * 
 */


#include <FastLED.h>
#include <math.h>
#include <esp_now.h>
#include <WiFi.h>
