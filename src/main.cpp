#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// PN532 SPI Pins
#define PN532_SCK 12
#define PN532_MOSI 20
#define PN532_MISO 8
#define PN532_SS 5

// IRQ interrupt pin to ESP32
#define PN532_IRQ 4

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

volatile bool nfcInterrupt = false;
uint8_t payload[32];
uint8_t payloadLen = 0;

void IRAM_ATTR onNfcIRQ()
{
  nfcInterrupt = true;
}

// put function declarations here:
int myFunction(int, int);

void setup()
{
  Serial.begin(115200);
  pinMode(PN532_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PN532_IRQ), onNfcIRQ, FALLING);

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.println("Did not found PN532");
    while (1)
      ;
  }
  nfc.SAMConfig();
  Serial.println('NFC ready');
}

void loop()
{
  if (!nfcInterrupt)
    return;
  nfcInterrupt = false;

  uint8_t uid[7];
  uint8_t uidLen;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 1000))
  {
    payloadLen = uidLen;
    memcpy(payload, uid, payloadLen);

    Serial.print('Payload:');
    for (uint8_t i = 0; i < payloadLen; i++)
    {
      Serial.print(payload[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
  else
  {
    Serial.println('Reading error or no tag');
  }
}