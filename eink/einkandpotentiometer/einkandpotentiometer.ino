// GxEPD2_HelloWorld.ino by Jean-Marc Zingg

// see GxEPD2_wiring_examples.h for wiring suggestions and examples
// if you use a different wiring, you need to adapt the constructor parameters!

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMono24pt7b.h>

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

// or select the display constructor line in one of the following files (old style):
#include "GxEPD2_display_selection.h"
#include "GxEPD2_display_selection_added.h"
#include "wi_sunrise_48x48new.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or GxEPD2_display_selection_added.h to here
// e.g. for Wemos D1 mini:
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0154D67

void setup()
{
  // initialize serial communication at 115200 bits per second:
//  Serial.begin(115200);
  
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  helloWorld();
  display.hibernate();
}

const char HelloWorld[] = "this reads potentiometers, potentially";
int sensorValue;

void helloWorld()
{
  display.setRotation(2);
  display.setFont(&FreeMono24pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  
  do
  {   
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.setTextWrap(true);
    display.print(HelloWorld);
    pinMode(34,INPUT);
    sensorValue = analogRead(34);
    display.print(sensorValue);
    display.drawInvertedBitmap(0, 204 + (48 + 8) * 0,
                             wi_sunrise_48x48, 48, 48, GxEPD_BLACK);
  }
  while (display.nextPage());
}

void loop() {
  
  // display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  //helloWorld();
  // display.hibernate();
  
};
