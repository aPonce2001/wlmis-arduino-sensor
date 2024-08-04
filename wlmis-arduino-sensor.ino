#include <RGBLed.h>

#define RED_PIN A0
#define GREEN_PIN A1
#define BLUE_PIN A2

RGBLed ledRGB(RED_PIN, GREEN_PIN, BLUE_PIN, RGBLed::COMMON_ANODE);

void setup()
{
}

void loop()
{
    ledRGB.setColor(RGBLed::RED);
    delay(500);
    ledRGB.setColor(RGBLed::YELLOW);
    delay(500);
    ledRGB.setColor(RGBLed::GREEN);
    delay(500);
    ledRGB.setColor(0, 0, 0);
    delay(500);
    ledRGB.setColor(155, 200, 142);
    delay(500);
    ledRGB.setColor(255, 255, 255);
    delay(500);
}
