#include <nfc.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <timer.h>

#define SDA_PIN 6
#define SCL_PIN 7
#define Sektor 2
#define DURATION 60000 // 1 minute in milliseconds
#define TIMEOUT 80     // 80 milliseconds timeout for NFC operations
uint8_t firstBlock;

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

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

UID wildCard;
uint8_t uid[7], uidLen;
uint8_t KEY_A[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t data[16];
Timer timer(DURATION);
Command cmd;

void nfc_setup()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    nfc.begin();
    nfc.SAMConfig();
    firstBlock = Sektor * 4; // Assuming each sector has 4 blocks
    Serial.begin(115200);
}

void nfc_loop()
{
    static uint32_t lastCycle = 0;
    const uint32_t cycleTime = 350; // milliseconds between NFC reads

    uint32_t now = millis();
    if (now - lastCycle >= cycleTime)
    {
        lastCycle = now;
        // Check if a card is present, and read its UID.
        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, TIMEOUT))
        {
            // Handle the wildcard logic.
            handleWildCard();
            // Read the first data block from the card.
            readDataBlock();
            memcpy(&cmd, data, sizeof(cmd));
            exec();
        }
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

void handleWildCard()
{
    // Check if the card is a wildcard
    if (isWildCard())
    {
        // If the wildcard is not set, set it to the current UID
        // Start the timer for DURATION.
        if (wildCard.uidLen == 0)
        {
            wildCard.uidLen = uidLen;
            memcpy(wildCard.uid, uid, uidLen);
            timer.start();
        }
        // If the wildcard is set, check if it matches the current uid.
        // Stop the timer if it matches.
        else if (wildCard.uid == uid)
        {
            resetWildCard();
            timer.stop();
        }
        // If the wildcard is set but does not match, return.
        else
        {
            return;
        }
    }
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
