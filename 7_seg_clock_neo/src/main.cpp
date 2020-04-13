// Retro 7 Segment Clock - Regular Edition - Software update v5
// https://www.thingiverse.com/thing:3014572

// Adapted for platformio - asaxen 2020

#define FASTLED_ALLOW_INTERRUPTS 0

#include <DS3232RTC.h>
#include <Wire.h>
#include <FastLED.h>
#include <TimeLib.h>
#include <EEPROM.h>

#define LED_PIN 6                                // led data in connected to d6
#define LED_PWR_LIMIT 750                        // 750mA - Power limit in mA (voltage is set in setup() to 5v)
#define LED_DIGITS 4                             // 4 or 6 digits, can only be an even number as...
#define LED_PER_DIGITS_STRIP 47                  // ...two digits are made out of one piece of led strip with 47 leds...
#define LED_BETWEEN_DIGITS_STRIPS 5              // 5 leds between above strips - and all this gives us LED_COUNT... :D
#define LED_COUNT ( LED_DIGITS / 2 ) * LED_PER_DIGITS_STRIP + ( LED_DIGITS / 3 ) * LED_BETWEEN_DIGITS_STRIPS

CRGB leds[LED_COUNT];

CRGBPalette16 currentPalette;

const bool dbg = true;                           // debug, true = enable serial input/output - set to false to save memory

int buttonA = 3;                                 // momentary push button, 1 pin to gnd, 1 pin to d3
int buttonB = 4;                                 // momentary push button, 1 pin to gnd, 1 pin to d4

byte brightness = 100;                           // default brightness if none saved to eeprom yet / first run
byte brightnessLevels[3] {100, 160, 240};        // 0 - 254, brightness Levels (min, med, max) - index (0-2) will get stored to eeprom
                                                 // Note: With brightnessAuto = 1 this will be the maximum brightness setting used!
byte brightnessAuto = 0;                         // 1 = enable brightness corrections using a photo resistor/readLDR();
byte upperLimitLDR = 140;                        // everything above this value will cause max brightness to be used (if it's higher than this)
byte lowerLimitLDR = 40;                         // everything below this value will cause minBrightness to be used
byte minBrightness = 20;                         // anything below this avgLDR value will be ignored
float factorLDR = 1.3;                           // try 0.5 - 2.0, compensation value for avgLDR. Set dbgLDR & dbg to true and watch serial console. Looking...
const bool dbgLDR = true;                       // ...for values in the range of 120-160 (medium room light), 40-80 (low light) and 0 - 20 in the dark
int pinLDR = 1;                                  // LDR connected to A1
byte intervalLDR = 60;                           // read value from LDR every 60ms (most LDRs have a minimum of about 30ms - 50ms)
unsigned long valueLDRLastRead = 0;              // time when we did the last readout
int avgLDR = 0;                                  // we will average this value somehow somewhere in readLDR();
int lastAvgLDR = 0;

byte startColor = 0;                             // "index" for the palette color used for drawing
byte displayMode = 0;                            // 0 = 12h, 1 = 24h (will be saved to EEPROM once set using buttons)
byte colorOffset = 32;                           // default distance between colors on the color palette used between digits/leds (in overlayMode)
int colorChangeInterval = 1500;                  // interval (ms) to change colors when not in overlayMode (per pixel/led coloring uses overlayInterval)
byte overlayMode = 0;                            // switch on/off (1/0) to use void colorOverlay(); (will be saved to EEPROM once set using buttons)
int overlayInterval = 200;                       // interval (ms) to change colors in overlayMode

byte btnRepeatCounter = 1;
byte lastKeyPressed = 0;
unsigned long btnRepeatStart = 0;

byte lastSecond = 0;
unsigned long lastLoop = 0;
unsigned long lastColorChange = 0;

/* these values will be stored to the EEPROM:
  0 = index for selectedPalette / switchPalette();
  1 = index for brightnessLevels / switchBrightness();
  2 = displayMode (when set using the buttons)
  3 = overlayMode (when set using the buttons)
*/

byte segGroups[14][2] = {         // 14 segments per strip, each segment has 1-x led(s). So lets assign them in a way we get something similar for both digits
  // right (seen from front) digit. This is which led(s) can be seen in which of the 7 segments (two numbers: First and last led inside the segment, same on TE):
  {  6,  8 },                     // top, a
  {  9, 11 },                     // top right, b
  { 13, 15 },                     // bottom right, c
  { 16, 18 },                     // bottom, d
  { 19, 21 },                     // bottom left, e
  {  3,  5 },                     // top left, f
  {  0,  2 },                     // center, g
  // left (seen from front) digit
  { 38, 40 },                     // top, a
  { 41, 43 },                     // top right, b
  { 25, 27 },                     // bottom right, c
  { 28, 30 },                     // bottom, d
  { 31, 33 },                     // bottom left, e
  { 35, 37 },                     // top left, f
  { 44, 46 }                      // center, g
};
// Note: The first number always has to be the lower one as they're subtracted later on... (fix by using abs()? ^^)

// Using above arrays it's very easy to "talk" to the segments. Simply use 0-6 for the first 7 segments, add 7 (7-13) for the following ones per strip/two digits
byte digits[10][7] = {                    // Lets define 10 numbers (0-9) with 7 segments each, 1 = segment is on, 0 = segment is off
  {   1,   1,   1,   1,   1,   1,   0 },  // 0 -> Show segments a - f, don't show g (center one)
  {   0,   1,   1,   0,   0,   0,   0 },  // 1 -> Show segments b + c (top and bottom right), nothing else
  {   1,   1,   0,   1,   1,   0,   1 },  // 2 -> and so on...
  {   1,   1,   1,   1,   0,   0,   1 },  // 3
  {   0,   1,   1,   0,   0,   1,   1 },  // 4
  {   1,   0,   1,   1,   0,   1,   1 },  // 5
  {   1,   0,   1,   1,   1,   1,   1 },  // 6
  {   1,   1,   1,   0,   0,   0,   0 },  // 7
  {   1,   1,   1,   1,   1,   1,   1 },  // 8
  {   1,   1,   1,   1,   0,   1,   1 }   // 9
};

void dbgInput() {
  if ( dbg ) {
    if ( Serial.available() > 0 ) {
      byte incomingByte = 0;
      incomingByte = Serial.read();
      if ( incomingByte == 44 ) if ( overlayMode == 0 ) overlayMode = 1; else overlayMode = 0;  // ,
      if ( incomingByte == 48 ) if ( displayMode == 0 ) displayMode = 1; else displayMode = 0;  // 0
      if ( incomingByte == 49 ) FastLED.setTemperature(OvercastSky);                            // 1
      if ( incomingByte == 50 ) FastLED.setTemperature(DirectSunlight);                         // 2 
      if ( incomingByte == 51 ) FastLED.setTemperature(Halogen);                                // 3
      if ( incomingByte == 52 ) overlayInterval = 30;                                           // 4
      if ( incomingByte == 53 ) colorChangeInterval = 10;                                       // 5
      if ( incomingByte == 54 ) { overlayInterval = 200; colorChangeInterval = 1500; }          // 6
      if ( incomingByte == 55 ) lastKeyPressed = 1;                                             // 7
      if ( incomingByte == 56 ) lastKeyPressed = 2;                                             // 8
      if ( incomingByte == 57 ) lastKeyPressed = 12;                                            // 9
      if ( incomingByte == 43 ) colorOffset += 16;                                              // +
      if ( incomingByte == 45 ) colorOffset -= 16;                                              // -
      Serial.print(F("Input - ASCII: ")); Serial.println(incomingByte, DEC); Serial.println();
    }
  }
}



void switchPalette() {                                          // Simply add palettes, make sure paletteCount increases accordingly
  byte paletteCount = 7;                                        // A few examples of gradients/solid colors by using RGB values or HTML Color Codes
  byte tmp = EEPROM.read(0);
  if ( dbg ) { Serial.print(F("switchPalette() EEPROM value: ")); Serial.println(tmp); }
  if ( tmp > paletteCount - 1 ) tmp = 0; else tmp = tmp;        // If value hasn't been written yet eeprom.read will return 255, in that case set tmp = 0
  static byte selectedPalette = tmp;
  switch ( selectedPalette ) {
    case 0: currentPalette = CRGBPalette16( CRGB( 224,   0,  32 ),
                                            CRGB(   0,   0, 244 ),
                                            CRGB( 128,   0, 128 ),
                                            CRGB( 224,   0,  64 ) ); break;
    case 1: currentPalette = CRGBPalette16( CRGB( 224,  16,   0 ),
                                            CRGB( 192,  64,   0 ),
                                            CRGB( 128, 128,   0 ),
                                            CRGB( 224,  32,   0 ) ); break;
    case 2: currentPalette = CRGBPalette16( CRGB::Aquamarine,
                                            CRGB::Turquoise,
                                            CRGB::Blue,
                                            CRGB::DeepSkyBlue   ); break;
    case 3: currentPalette = RainbowColors_p; break;
    case 4: currentPalette = PartyColors_p; break;
    case 5: currentPalette = CRGBPalette16( CRGB::White ); break;
    case 6: currentPalette = CRGBPalette16( CRGB::LawnGreen ); break;
  }
  EEPROM.write(0, selectedPalette);
  if (dbg) { Serial.print(F("switchPalette() EEPROM write: ")); Serial.println(selectedPalette); Serial.println(); }
  if (selectedPalette < paletteCount - 1) selectedPalette++; else selectedPalette = 0;
}

void switchBrightness() {
  byte tmp = EEPROM.read(1);
  if ( dbg ) { Serial.print(F("switchBrightness() EEPROM value: ")); Serial.println(tmp); }
  if ( tmp > 2 ) tmp = 0;
  static byte selectedBrightness = tmp;
  switch ( selectedBrightness ) {
    case 0: brightness = brightnessLevels[selectedBrightness]; break;
    case 1: brightness = brightnessLevels[selectedBrightness]; break;
    case 2: brightness = brightnessLevels[selectedBrightness]; break;
  }
  EEPROM.write(1, selectedBrightness); 
  if ( dbg ) { Serial.print(F("switchBrightness() EEPROM write: ")); Serial.println(selectedBrightness); Serial.println(); }
  if ( selectedBrightness < 2 ) selectedBrightness++; else selectedBrightness = 0;
}

DEFINE_GRADIENT_PALETTE (setupColors_gp) {                                                  // this color palette will only be used while in setup
    1, 255,   0,   0,                                                                       // unset values = red, current value = yellow, set values = green
   63, 255, 255,   0,
  127,   0, 255,   0,
  191,   0, 255, 255,
  255,   0,   0, 255
};


void loadValuesFromEEPROM() {
  byte tmp = EEPROM.read(2);
  if ( tmp != 255 ) displayMode = tmp;                          // if no values have been stored to eeprom at position 2/3 then a read will give us 255...
  tmp = EEPROM.read(3);                                         // ...so we'll only take the value from eeprom if it's different from 255
  if ( tmp != 255 ) overlayMode = tmp;                          // (for colorMode/displayMode which can only bei 0/1)
}

void readLDR() {                                                                                            // read LDR value 5 times and write average to avgLDR for use in updateDisplay();
  static byte runCounter = 1;
  static int tmp = 0;
  byte readOut = map(analogRead(pinLDR), 0, 1023, 0, 250);
  tmp += readOut;
  if (runCounter == 5) {
    avgLDR = (tmp / 5)  * factorLDR;
    tmp = 0; runCounter = 0;
    if ( dbg && dbgLDR ) { Serial.print(F("avgLDR value: ")); Serial.print(avgLDR); }
    avgLDR = max(avgLDR, minBrightness); avgLDR = min(avgLDR, brightness);                                  // this keeps avgLDR in a range between minBrightness and maximum current brightness
    if ( avgLDR >= upperLimitLDR && avgLDR < brightness ) avgLDR = brightness;                              // if avgLDR is above upperLimitLDR switch to max current brightness
    if ( avgLDR <= lowerLimitLDR ) avgLDR = minBrightness;                                                  // if avgLDR is below lowerLimitLDR switch to minBrightness
    if ( dbg && dbgLDR ) { Serial.print(F(" - adjusted to: ")); Serial.println(avgLDR); }
  }
  runCounter++;
}

void colorOverlay() {                                                                                       // This "projects" colors on already drawn leds before showing leds in updateDisplay();
  for (byte i = 0; i < LED_COUNT; i++) {                                                                    // check each led...
    if (leds[i])                                                                                            // ...and if it is lit...
      leds[i] = ColorFromPalette(currentPalette, startColor + (colorOffset * i), brightness, LINEARBLEND);  // ...assign increasing color from current palette
  }
}

byte readButtons() {
  byte activeButton = 0;
  byte retVal = 0;
  static int btnRepeatDelay = 150;
  static unsigned long lastButtonPress = 0;
  if ( digitalRead(buttonA) == 0 || digitalRead(buttonB) == 0) {
    if (digitalRead(buttonA) == 0) activeButton = 1;
    else if (digitalRead(buttonB) == 0) activeButton = 2;
    if ( digitalRead(buttonA) == 0 && digitalRead(buttonB) == 0 ) activeButton = 12;
    if (millis() - lastButtonPress >= btnRepeatDelay) {
      btnRepeatStart = millis();
      btnRepeatCounter = 0;
      retVal = activeButton;
    } else if (millis() - btnRepeatStart >= btnRepeatDelay * (btnRepeatCounter + 1) ) {
      btnRepeatCounter++;
      if (btnRepeatCounter > 5) retVal = activeButton;
    }
    lastButtonPress = millis();
  }
  return retVal;
}

void showSegment(byte segment, byte color, byte segDisplay) {
  // This shows the segments from top of the sketch on a given position (segDisplay).
  // pos 0 is the most right one (seen from the front) where data in is connected to the arduino
  byte leds_per_segment = 1 + abs( segGroups[segment][1] - segGroups[segment][0] );            // get difference between 2nd and 1st value in array to get led count for this segment
  if ( segDisplay % 2 != 0 ) segment += 7;                                                  // if segDisplay/position is odd add 7 to segment
  for (byte i = 0; i < leds_per_segment; i++) {                                             // fill all leds inside current segment with color
    leds[( segGroups[segment][0] + ( segDisplay / 2 ) * ( LED_PER_DIGITS_STRIP + LED_BETWEEN_DIGITS_STRIPS ) ) + i] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
  }
}

void showDigit(byte digit, byte color, byte pos) {
  // This draws numbers using the according segments as defined on top of the sketch (0 - 9)
  for (byte i = 0; i < 7; i++) {
    if (digits[digit][i] != 0) showSegment(i, color, pos);
  }
}

void showDots(byte dots, byte color) {
  // in 12h mode and while in setup upper dots resemble AM, all dots resemble PM
  byte startPos = LED_PER_DIGITS_STRIP;
  if ( LED_BETWEEN_DIGITS_STRIPS % 2 == 0 ) {                                                                 // only SE/TE should have a even amount here (0/2 leds between digits)
    leds[startPos] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
    if ( dots == 2 ) leds[startPos + 1] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
  } else {                                                                                                    // Regular and XL have 5 leds between digits
    leds[startPos] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
    leds[startPos + 1] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
    if ( LED_DIGITS / 3 > 1 ) {
        leds[startPos + LED_PER_DIGITS_STRIP + LED_BETWEEN_DIGITS_STRIPS] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
        leds[startPos + LED_PER_DIGITS_STRIP + LED_BETWEEN_DIGITS_STRIPS + 1] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
      }
    if ( dots == 2 ) {
      leds[startPos + 3] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
      leds[startPos + 4] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
      if ( LED_DIGITS / 3 > 1 ) {
        leds[startPos + LED_PER_DIGITS_STRIP + LED_BETWEEN_DIGITS_STRIPS + 3] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
        leds[startPos + LED_PER_DIGITS_STRIP + LED_BETWEEN_DIGITS_STRIPS + 4] = ColorFromPalette(currentPalette, color, brightness, LINEARBLEND);
      }
    }
  }
}

void displayTime(time_t t, byte color, byte colorSpacing) {
  byte posOffset = 0;                                                                     // this offset will be used to move hours and minutes...
  if ( LED_DIGITS / 2 > 2) posOffset = 2;                                                 // ... to the left so we have room for the seconds when there's 6 digits available
  if ( displayMode == 0 ) {                                                               // if 12h mode is selected...
    if ( hourFormat12(t) >= 10 ) showDigit(1, color + colorSpacing * 2, 3 + posOffset);   // ...and hour > 10, display 1 at position 3
    showDigit((hourFormat12(t) % 10), color + colorSpacing * 3, 2  + posOffset);          // display 2nd digit of HH anyways
  } else if ( displayMode == 1 ) {                                                        // if 24h mode is selected...
    if ( hour(t) > 9 ) showDigit(hour(t) / 10, color + colorSpacing * 2, 3 + posOffset);  // ...and hour > 9, show 1st digit at position 3 (this is to avoid a leading 0 from 0:00 - 9:00 in 24h mode)
    showDigit(hour(t) % 10, color + colorSpacing * 3, 2 + posOffset);                     // again, display 2nd digit of HH anyways
  }
  showDigit((minute(t) / 10), color + colorSpacing * 4, 1 + posOffset);                   // minutes thankfully don't differ between 12h/24h, so this can be outside the above if/else
  showDigit((minute(t) % 10), color + colorSpacing * 5, 0 + posOffset);                   // each digit is drawn with an increasing color (*2, *3, *4, *5) (*6 and *7 for seconds only in HH:MM:SS)
  if ( posOffset > 0 ) {
    showDigit((second(t) / 10), color + colorSpacing * 6, 1);
    showDigit((second(t) % 10), color + colorSpacing * 7, 0);
  }
  if ( second(t) % 2 == 0 ) showDots(2, second(t) * 4.25);                                // show : between hours and minutes on even seconds with the color cycling through the palette once per minute
  lastSecond = second(t);
}

void updateDisplay(byte color, byte colorSpacing) {                                                         // this is what redraws the "screen"
  FastLED.clear();                                                                                          // clear whatever the leds might have assigned currently...
  displayTime(now(), color, colorSpacing);                                                                  // ...set leds to display the time...
  if (overlayMode == 1) colorOverlay();                                                                     // ...and if using overlayMode = 1 draw custom colors over single leds
  if (brightnessAuto == 1) {                                                                                // If brightness is adjusted automatically by using readLDR()...
    FastLED.setBrightness(avgLDR);                                                                          // ...set brightness to avgLDR
  } else {                                                                                                  // ...otherwise...
    FastLED.setBrightness(brightness);                                                                      // ...assign currently selected brightness
  }
}

void setupClock() {
  // finally not using a custom displayTime routine for setup, improvising a bit and using the setupColor-Palette defined on top of the sketch
  if ( dbg ) Serial.println(F("Entering setup mode..."));
  byte prevBrightness = brightness;                                                         // store current brightness and switch back after setup
  brightness = brightnessLevels[1];                                                         // select medium brightness level
  currentPalette = setupColors_gp;                                                          // use setupColors_gp palette while in setup
  tmElements_t setupTime;                                                                   // Create a time element which will be used. Using the current time would
  setupTime.Hour = 12;                                                                      // give some problems (like time still running while setting hours/minutes)
  setupTime.Minute = 0;                                                                     // Setup starts at 12 (12 pm)
  setupTime.Second = 1;                                                                     // 1 because displayTime() will always display both dots at even seconds
  setupTime.Day = 20;                                                                       // not really neccessary as day/month aren't used but who cares ^^
  setupTime.Month = 9;                                                                      // see above
  setupTime.Year = 2019 - 1970;                                                             // yes... .Year is set by the difference since 1970. So "49" is what we want.
  byte setting = 1;                                                                         // counter to keep track of what's currently adjusted: 1 = hours, 2 = minutes
  byte blinkStep = 0;
  int blinkInterval = 500;
  unsigned long lastBlink = millis();
  FastLED.clear();
  FastLED.show();
  while ( digitalRead(buttonA) == 0 || digitalRead(buttonB) == 0 ) delay(20);               // this will keep the display blank while any of the keys is still pressed
  while ( setting <= LED_DIGITS / 2 ) {                                                     // 2 - only setup HH:MM. 3 - setup HH:MM:SS
    if ( lastKeyPressed == 1 ) setting += 1;                                                // one button will accept the current setting and proceed to the next one...
    if ( lastKeyPressed == 2 )                                                              // while the other button increases the current value
      if ( setting == 1 )                                                                   // if setting = 1 ...
        if ( setupTime.Hour < 23 ) setupTime.Hour += 1; else setupTime.Hour = 0;            // ...increase hour when buttonB is pressed
      else if ( setting == 2 )                                                              // else if setting = 2...
        if (setupTime.Minute < 59) setupTime.Minute += 1; else setupTime.Minute = 0;        // ...increase minute when buttonB is pressed
      else if ( setting == 3 )                                                              // else if setting = 3...
        if (setupTime.Second < 59) setupTime.Second += 1; else setupTime.Second = 0;        // ...increase second when buttonB is pressed
    if ( millis() - lastBlink >= blinkInterval ) {                                          // pretty sure there is a much easier and nicer way...
      if ( blinkStep == 0 ) { brightness = brightnessLevels[2]; blinkStep = 1; }            // ...to get the switch between min and max brightness (boolean?)
        else { brightness = brightnessLevels[0]; blinkStep = 0; }                           
      lastBlink = millis();
    }
    FastLED.clear();
    // Still a mess. But displayTime takes care of am/pm and positioning, so this draws the time
    // and redraws certain leds in different colors while in each setting
    // Maybe use the colorOffset in displayTime and manipulate/prepare palette only inside setup?
    if ( setting == 1 ) {
      displayTime(makeTime(setupTime), 63, 0);
      for ( byte i = 0; i < ( ( LED_DIGITS / 3 ) * LED_PER_DIGITS_STRIP + ( LED_DIGITS / 3 * LED_BETWEEN_DIGITS_STRIPS ) ); i++ ) 
        if ( leds[i] ) leds[i] = ColorFromPalette(currentPalette, 1, brightness, LINEARBLEND);
      if ( ( setupTime.Hour < 12 ) && ( displayMode == 0 ) ) showDots(1, 191); else showDots(2, 191);
    } else if ( setting == 2 ) {
      displayTime(makeTime(setupTime), 95, 0);
      for ( byte i = 0; i < ( ( LED_DIGITS / 3 ) * LED_PER_DIGITS_STRIP + ( LED_DIGITS / 3 * LED_BETWEEN_DIGITS_STRIPS ) ); i++ )
        if ( leds[i] ) leds[i] = ColorFromPalette(currentPalette, 63, brightness, LINEARBLEND);
      if ( LED_DIGITS / 3 > 1 ) for ( byte i = 0; i < LED_PER_DIGITS_STRIP; i++ )
        if ( leds[i] ) leds[i] = ColorFromPalette(currentPalette, 1, brightness, LINEARBLEND);
      if ( ( setupTime.Hour < 12 ) && ( displayMode == 0 ) ) showDots(1, 191); else showDots(2, 191);
    } else if ( setting == 3 ) {
      displayTime(makeTime(setupTime), 95, 0);
      for ( byte i = 0; i < LED_PER_DIGITS_STRIP; i++ )
        if ( leds[i] ) leds[i] = ColorFromPalette(currentPalette, 63, brightness, LINEARBLEND);
      if ( ( setupTime.Hour < 12 ) && ( displayMode == 0 ) ) showDots(1, 191); else showDots(2, 191);
    }
    FastLED.show();
    lastKeyPressed = readButtons();
    if ( dbg ) dbgInput();
  }
  RTC.write(setupTime);                                                                     // Finally, write setupTime to the rtc
  FastLED.clear(); displayTime(makeTime(setupTime), 95, 0); FastLED.show();
  time_t sync = now();                                                                      // create variable sync to synchronize arduino clock to rtc
  brightness = prevBrightness;
  switchPalette();
  delay(500);
  for ( byte i = 0; i < 255; i++ ) { for ( byte x = 0; x < LED_COUNT; x++ ) leds[x]--; FastLED.show(); delay(2); }  // short delay followed by fading all leds to black
  if ( dbg ) Serial.println(F("Setup done..."));
}


void setup() {
  if (brightnessAuto == 1) pinMode(pinLDR, OUTPUT);
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  if (dbg) {
    Serial.begin(57600); Serial.println();    
    Serial.println(F("7 Segment-Clock v5 starting up..."));
    Serial.print(F("Configured for: ")); Serial.print(LED_COUNT); Serial.println(F(" leds"));
    Serial.print(F("Power limited to (mA): ")); Serial.print(LED_PWR_LIMIT); Serial.println(F(" mA")); Serial.println();
  }
  Serial.println("aaa");
  Wire.begin();
  Serial.println("bb");
  setSyncProvider(RTC.get);
  Serial.println("bbb");
  setSyncInterval(15);
  Serial.println("a");
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT).setCorrection(TypicalSMD5050).setTemperature(DirectSunlight).setDither(1);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_PWR_LIMIT);
  FastLED.clear();
  FastLED.show();
  Serial.println("b");
  loadValuesFromEEPROM();
  switchPalette();
  switchBrightness();
  Serial.println("c");
}

void loop() {
  if (  ( lastLoop - lastColorChange >= colorChangeInterval ) && ( overlayMode == 0 )         // if colorChangeInterval has been reached and overlayMode is disabled...
     || ( lastLoop - lastColorChange >= overlayInterval ) && ( overlayMode == 1 ) ) {         // ...or if overlayInterval has been reached and overlayMode is enabled...
    startColor++;                                                                             // increase startColor to "move" colors slowly across the digits/leds
    updateDisplay(startColor, colorOffset);
    lastColorChange = millis();
  }
  if ( lastSecond != second() ) {                                                             // if current second is different from last second drawn...
    updateDisplay(startColor, colorOffset);                                                   // lastSecond will be set in displayTime() and will be used for
    lastSecond = second();                                                                    // redrawing regardless the digits count (HH:MM or HH:MM:SS)
  }
  if ( lastKeyPressed == 1 ) {                                                                // if buttonA is pressed...
    switchBrightness();                                                                       // ...switch to next brightness level
    updateDisplay(startColor, colorOffset);
    if ( btnRepeatCounter >= 20 ) {                                                           // if buttonA is held for a few seconds change overlayMode 0/1 (using colorOverlay())
      if ( overlayMode == 0 ) overlayMode = 1; else overlayMode = 0;
      updateDisplay(startColor, colorOffset);
      EEPROM.write(3, overlayMode);                                                           // ...and write setting to eeprom
      btnRepeatStart = millis();
    }
  }
  if ( lastKeyPressed == 2 ) {                                                                // if buttonB is pressed...
    switchPalette();                                                                          // ...switch between color palettes
    updateDisplay(startColor, colorOffset);
    if ( btnRepeatCounter >= 20 ) {                                                           // if buttonB is held for a few seconds change displayMode 0/1 (12h/24h)...
      if ( displayMode == 0 ) displayMode = 1; else displayMode = 0;
      updateDisplay(startColor, colorOffset);
      EEPROM.write(2, displayMode);                                                           // ...and write setting to eeprom
      btnRepeatStart = millis();
    }
  }
  if ( ( lastLoop - valueLDRLastRead >= intervalLDR ) && ( brightnessAuto == 1 ) ) {          // if LDR is enabled and sample interval has been reached...
    readLDR();                                                                                // ...call readLDR();
    if ( abs(avgLDR - lastAvgLDR) >= 5 ) {                                                    // only adjust current brightness if avgLDR has changed for more than +/- 5.
      updateDisplay(startColor, colorOffset);
      lastAvgLDR = avgLDR;
      if ( dbg ) { Serial.print(F("Updated display with avgLDR of: ")); Serial.println(avgLDR); }
    }
    valueLDRLastRead = millis();
  }
  FastLED.show();                                                                             // run FastLED.show() every time to avoid color flickering at low brightness settings
  if ( lastKeyPressed == 12 ) setupClock();                                                   // enter setup
  lastKeyPressed = readButtons();
  lastLoop = millis();
  if ( dbg ) dbgInput();                                                                      // if dbg = true this will read serial input/keys
}







