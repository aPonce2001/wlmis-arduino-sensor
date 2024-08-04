#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("RFID Iniciado");
}

void loop()
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    mfrc522.PICC_HaltA();
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