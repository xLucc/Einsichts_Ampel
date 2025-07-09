#include <Arduino.h>
#include <Adafruit_PN532.h>

// Pin setup

#define I2C_SDA 6
#define I2C_SCL 7
#define PN532_IRQ 5
#define PN532_RESET -1

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

volatile bool interrupt = false;
uint8_t KEY_A[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t data[4][16];

// Interrupt

void IRAM_ATTR
irqHandler()
{
  interrupt = true;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("NFC Reader init...");

  // Initialize the reader
  nfc.begin();

  // Infinite loop if an error occur.
  if (!nfc.getFirmwareVersion())
  {
    Serial.println("Connection-Error");
    while (1)
      ;
  }

  // Start the reader.
  nfc.SAMConfig();

  // Set the interrupt pin.
  pinMode(PN532_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PN532_IRQ), irqHandler, FALLING);

  pinMode(8, OUTPUT);

  Serial.println("Waiting for cards.");
}

void loop()
{
  if (interrupt)
  {
    interrupt = false;
    uint8_t uid[7], uidLen;

    // Check the uid, if it's valid.
    if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 0))
      return;

    // Convert the uid into a string.
    String uidStr = "";
    for (uint8_t i = 0; i < uidLen; i++)
    {
      if (uid[i] < 0X10)
        uidStr += "0";

      uidStr += String(uid[i], HEX);
    }

    digitalWrite(8, HIGH);
  }
}
