#include <Arduino.h>
#include <Adafruit_PN532.h>

// Pin setup

#define I2C_SDA 8
#define I2C_SCL 9
#define PN532_IRQ 10
#define PN532_RESET -1

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

volatile bool interrupt = false;
uint8_t KEY_A[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Interrupt

void IRAM_ATTR irqHandler() { interrupt = true; }

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Wire.begin(I2C_SDA, I2C_SCL, 400000); // I2C @ 400kHz
  nfc.begin();
  nfc.SAMConfig();

  pinMode(PN532_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PN532_IRQ), irqHandler, FALLING);

  Serial.println('Waiting for Commands.');
}

void loop()
{
  if (interrupt)
  {
    interrupt = false;
    dumpTag();
  }
}

void dumpTag()
{
  uint8_t uid[7], uidLen;

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 0))
  {
    return;
  }

  // Read the UID.
  Serial.printf("\nUID: ");
  for (uint8_t i = 0; i < uidLen; i++)
  {
    if (uid[i] < 0x10)
    {
      Serial.print('0');
    }
    Serial.print(uid[i], HEX);
    Serial.print(' ');
  }

  Serial.println();

  uint8_t data[16];

  // Read all the Blocks.
  for (uint8_t block = 0; block < 64; block++)
  {
    // Authenticate the Block.
    if (block % 4 == 0)
    {
      if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLen, block, 0, KEY_A))
      {
        Serial.printf("Auth-Error Sector %u\n", block / 4);
        return;
      }
    }

    // Read the Block
    if (nfc.mifareclassic_ReadDataBlock(block, data))
    {
      Serial.printf("Block %02u: ", block);
      for (uint8_t i = 0; i < 16; i++)
      {
        if (data[i] < 0x10)
          Serial.print('0');
        Serial.print(data[i], HEX);
        Serial.print(' ');
      }
      Serial.println();
    }
    else
    {
      Serial.printf("Reading-Error Block %u\n", block);
    }
  }
}
