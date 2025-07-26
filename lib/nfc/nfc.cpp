#include <nfc.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <timer.h>
#include <led.h>

#define SDA_PIN 6
#define SCL_PIN 7
#define RESET 5
#define Sektor 2
#define DURATION 60000 // 1 minute in milliseconds
#define TIMEOUT 200    // 200 milliseconds timeout for NFC operations
uint8_t firstBlock;
bool compareUID(uint8_t *uid1, uint8_t len1, uint8_t *uid2, uint8_t len2);

struct UID
{
    uint8_t uid[7];
    uint8_t uidLen;

    bool operator==(const UID &other) const
    {
        if (uidLen != other.uidLen)
            return false;
        for (uint8_t i = 0; i < uidLen; i++)
        {
            if (uid[i] != other.uid[i])
                return false;
        }
        return true;
    }
};

// This struct represents the command structure given by the NFC chip.
// It contains one block of data, which is 16 bytes long.
struct Command
{
    uint8_t function;   // First byte is the function code.
    uint32_t timeValue; // Next four bytes are the time value.
    uint8_t red;        // Next three bytes are the RGB color values.
    uint8_t green;
    uint8_t blue;
    uint8_t reserved[4]; // Reserved for future use.
};

Adafruit_PN532 nfc(-1, RESET, &Wire);

UID wildCard;
uint8_t uid[7], uidLen;
uint8_t KEY_A[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t data[16];
Timer timer(DURATION);
Command cmd;

void nfc_setup()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, LOW);
    delay(10);
    digitalWrite(RESET, HIGH);
    delay(10);
    nfc.begin();
    nfc.SAMConfig();
    firstBlock = Sektor * 4; // Assuming each sector has 4 blocks
    Serial.begin(115200);
}

void nfc_loop()
{
    static uint32_t lastCycle = 0;
    const uint32_t cycleTime = 200; // milliseconds between NFC reads

    uint32_t now = millis();
    if (now - lastCycle >= cycleTime)
    {
        lastCycle = now;
        nfc.reset();

        // Check if a card is present, and read its UID.
        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, TIMEOUT))
        {
            Serial.println("If this does not show up, i2c is working.");
            // Handle the wildcard logic.
            if (!myHandleWildCard())
            {
                // Read the first data block from the card.
                readDataBlock();
                memcpy(&cmd, data, sizeof(cmd));
                exec();
            }
        }

        Serial.println("If this is the only thing that shows up, i2c does not work.");
    }
    timer.tick();
}

// @todo add error handling for the case where the card is not readable, so if max retries are reached.
void readDataBlock()
{
    const int maxRetries = 5;
    int attempts = 0;
    while (attempts < maxRetries)
    {
        if (nfc.mifareclassic_ReadDataBlock(firstBlock, data))
        {
            return;
        }
        attempts++;
    }
}

void resetWildCard()
{
    memset(wildCard.uid, 0, sizeof(wildCard.uid));
    wildCard.uidLen = 0;
}

bool myHandleWildCard()
{
    // Check if the card is a wildcard
    if (isWildCard())
    {
        // If the wildcard is not set, set it to the current UID
        // Start the timer for DURATION.
        if (wildCard.uidLen == 0)
        {
            Serial.println("Case 1");
            wildCard.uidLen = uidLen;
            memcpy(wildCard.uid, uid, uidLen);
            timer.start();
            return true;
        }
        // If the wildcard is set, check if it matches the current uid.
        // Stop the timer if it matches.
        if (compareUID(wildCard.uid, wildCard.uidLen, uid, uidLen))
        {
            Serial.println("Case 2");
            resetWildCard();
            timer.stop();
            return true;
        }
        // If the wildcard is set but does not match, return.
        else
        {
            Serial.println("Case 3");
            return true;
        }
    }

    return false;
}

bool isWildCard()
{
    return nfc.mifareclassic_AuthenticateBlock(uid, uidLen, firstBlock, 0, KEY_A);
}

void exec()
{
    switch (cmd.function)
    {
    case 0x01: // Case for adding time
        timer.add(cmd.timeValue, {cmd.red, cmd.green, cmd.blue});
        break;
    case 0x02: // Case for pausing the timer.
        timer.pause({cmd.red, cmd.green, cmd.blue});
        break;
    case 0x03: // Case for resuming the timer.
        timer.resume({cmd.red, cmd.green, cmd.blue});
        break;
    default:
        break;
    }
}

bool compareUID(uint8_t *uid1, uint8_t len1, uint8_t *uid2, uint8_t len2)
{
    if (len1 != len2)
        return false;
    for (uint8_t i = 0; i < len1; i++)
    {
        if (uid1[i] != uid2[i])
            return false;
    }
    return true;
}

bool kill_timer()
{
    static uint32_t lastCycle = 0;
    const uint32_t cycleTime = 100; // milliseconds between NFC reads

    uint32_t now = millis();
    if (now - lastCycle >= cycleTime)
    {
        lastCycle = now;
        nfc.reset();

        // Check if a card is present, and read its UID.
        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, TIMEOUT))
        {
            // Handle the wildcard logic.
            if (isWildCard)
            {
                return compareUID(wildCard.uid, wildCard.uidLen, uid, uidLen);
            }
        }
    }
}
