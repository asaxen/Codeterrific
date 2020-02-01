
/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "Arduino.h"
#include <NeoPixelBus.h>

#define colorSaturation 255

const uint16_t PixelCount = 3;
const uint16_t PixelPin = 3;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);




void setup()
{
    pinMode( BUILTIN_LED, OUTPUT );
  // initialize LED digital pin as an output.
    Serial.begin(115200);
    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();
    strip.Begin();
    strip.Show();
}

void loop()
{
  delay(1000);
  digitalWrite(BUILTIN_LED, HIGH);
  strip.SetPixelColor(0, white);
  strip.SetPixelColor(1, white);
  strip.SetPixelColor(2, white);
  strip.Show();
  delay(1000);
  digitalWrite(BUILTIN_LED, LOW);

}