#include <RGBLed.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// RGB LED
#define RED_PIN A0
#define GREEN_PIN A1
#define BLUE_PIN A2
RGBLed ledRGB(RED_PIN, GREEN_PIN, BLUE_PIN, RGBLed::COMMON_ANODE);

// RFID
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

// LCD
#define I2C_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

// Ultrasonic sensor
#define TRIGGER_PIN 5
#define ECHO_PIN 6

// Water monitor struct
struct WaterLevel
{
    float heightCm;
    float volumeMl;
    float percent;
};

// Water monitor constants
const float RECIPIENT_HEIGHT_CM = 100.0f;
const float RECIPIENT_AREA_CM2 = 100.0f;

// Water monitor record
WaterLevel waterLevel = {0.0f, 0.0f, 0.0f};
float heightCm;

// State machine
enum State
{
    IDLE,
    MONITORING
};

State state = IDLE;

const byte rfidAuthorizedCard[4] = {0x00, 0x00, 0x00, 0x00};
byte rfidReadCard[4];

void setup()
{
    Serial.begin(9600);
    // Initialize SPI bus
    SPI.begin();
    mfrc522.PCD_Init();

    // Initialize I2C
    lcd.begin();
    lcd.backlight();

    // Ultrasonic sensor
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop()
{
    switch (state)
    {
    case IDLE:
        waitForRfid();
        break;
    case MONITORING:
        collectData();
        break;
    }
}

void waitForRfid()
{
    ledRGB.off();
    state = MONITORING;
}

void readRfid()
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        rfidReadCard[i] = mfrc522.uid.uidByte[i];
    }

    if (isArrayEqual(rfidReadCard, rfidAuthorizedCard))
    {
        ledRGB.setColor(RGBLed::GREEN);
        printLcd("---Autorizado---", " Inicio sistema");
        Serial.println("sensor-state");

        if (state == MONITORING)
        {
            state = IDLE;
            return;
        }

        state = MONITORING;
    }
    else
    {
        printLcd("----Denegado-----", "Intenta de nuevo");
        ledRGB.setColor(RGBLed::RED);
    }
}

bool isArrayEqual(const byte array1[], const byte array2[])
{
    if (sizeof(array1) != sizeof(array2))
    {
        return false;
    }

    for (int i = 0; i < sizeof(array1); ++i)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }
    return true;
}

void collectData()
{
    measureHeight();
    // TODO: Update LCD
    // TODO: Add 8x8 led matrix
    // TODO: Timer 1 for 500ms to send the json data in Serial
    readRfid();
}

void measureHeight()
{
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = 0.017f * duration;
    heightCm = distance;
    recordData(heightCm);
}

void recordData(float distance)
{
    waterLevel.heightCm = RECIPIENT_HEIGHT_CM - distance;
    waterLevel.volumeMl = waterLevel.heightCm * RECIPIENT_AREA_CM2;
    waterLevel.percent = waterLevel.heightCm / RECIPIENT_HEIGHT_CM;
}

void SendJsonDataSerial()
{
    Serial.print("{\"heightCm\":");
    Serial.print(waterLevel.heightCm);
    Serial.print(",\"volumeMl\":");
    Serial.print(waterLevel.volumeMl);
    Serial.print(",\"percent\":");
    Serial.print(waterLevel.percent);
    Serial.println("}");
}

void printLcd(String topMessage, String bottomMessage)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(topMessage);
    lcd.setCursor(0, 1);
    lcd.print(bottomMessage);
}