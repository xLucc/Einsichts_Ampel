#include <led.h>
#include <timer.h>
#include <FastLED.h>

#define DATA_PIN 6
#define BRIGHTNESS 100
#define NUM_LEDS 64
#define NUM_ROWS 8
#define NUM_COLS 8

CRGB leds[NUM_LEDS];

// This function initializes the LED strip by setting up the FastLED library.
// It configures the data pin, number of LEDs, and brightness level.
void led_setup()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

// This function iterates over all available LEDs and assigns each one the color
// provided in the RGBColor structure. After setting the colors, it calls FastLED.show()
// to apply the changes to the physical LEDs.
void show_color(RGBColor color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(color.r, color.g, color.b);
    }
    FastLED.show();
}

// This function receives a color identifier and shows it on the LEDs.
// The identifier is a simple RGBColor value, provided by the NFC Chip, based on the command, its payload sends.
// It shows the color as baselayer and the identifier as moving pattern.
void show_color_identifier(RGBColor color, RGBColor identifier)
{
    // Show the base color
    show_color(color);

    // Move the identifier color row by row (each row has 8 LEDs), bidirectionally
    int numRows = NUM_LEDS / NUM_ROWS;
    // Forward direction
    for (int row = 0; row < numRows; row++)
    {
        show_color(color);
        for (int col = 0; col < NUM_COLS; col++)
        {
            int idx = row * NUM_COLS + col;
            leds[idx] = CRGB(identifier.r, identifier.g, identifier.b);
        }
        FastLED.show();
    }
    // Reverse direction
    for (int row = numRows - 1; row >= 0; row--)
    {
        show_color(color);
        for (int col = 0; col < NUM_COLS; col++)
        {
            int idx = row * NUM_COLS + col;
            leds[idx] = CRGB(identifier.r, identifier.g, identifier.b);
        }
        FastLED.show();
    }
}

void show_color_wave(RGBColor color)
{
    // Create a wave effect by moving the color across the LEDs
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(color.r, color.g, color.b);
        FastLED.show();
        leds[i] = CRGB::Black;
    }
}