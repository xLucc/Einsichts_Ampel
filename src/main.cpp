#include <Arduino.h>
#include <nfc.h>
#include <led.h>

#define I2C_SDA 6
#define I2C_SCL 7
#define PN532_IRQ -1
#define PN532_RESET -1

void setup()
{
    nfc_setup();
    led_setup();
}

void loop()
{
    nfc_loop();
}
