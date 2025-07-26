#include <led.h>
#include <timer.h>
#include <FastLED.h>

#define DATA_PIN 4
#define BRIGHTNESS 5
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
    FastLED.clear();
    FastLED.show();
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
    unsigned long start = millis();
    while (millis() - start < 50)
    {
        yield();
    }
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
        // show_color(color);
        for (int col = 0; col < NUM_COLS; col++)
        {
            int idx = row * NUM_COLS + col;
            leds[idx] = CRGB(identifier.r, identifier.g, identifier.b);
        }
        FastLED.show();

        unsigned long startDelay = millis();
        while (millis() - startDelay < 75)
        {
            yield();
        }
    }
    // Reverse direction
    for (int row = numRows - 1; row >= 0; row--)
    {
        // show_color(color);
        for (int col = 0; col < NUM_COLS; col++)
        {
            int idx = row * NUM_COLS + col;
            leds[idx] = CRGB(color.r, color.g, color.b);
        }
        FastLED.show();

        unsigned long startDelay = millis();
        while (millis() - startDelay < 75)
        {
            yield();
        }
    }
}

// This function creates a wave effect on the LEDs.
void show_color_wave(RGBColor color)
{
    const int waveSize = 8;
    const unsigned long waveDelay = 80; // milliseconds

    int numWaves = NUM_LEDS / waveSize;

    for (int wave = 0; wave < numWaves; wave++)
    {
        // Clear all LEDs
        FastLED.clear();

        // Light up the current wave of 8 LEDs (full color)
        for (int i = 0; i < waveSize; i++)
        {
            int idx = wave * waveSize + i;
            if (idx < NUM_LEDS)
            {
                leds[idx] = CRGB(color.r, color.g, color.b);
            }
        }

        // Blur before (previous 8 LEDs)
        for (int i = 1; i <= waveSize; i++)
        {
            int idx = wave * waveSize - i;
            if (idx >= 0)
            {
                // Fade out: linear fade, more distant = dimmer
                uint8_t fade = map(i, 1, waveSize * 2, 100, 5); // 100 (close) to 5 (far)
                leds[idx] = CRGB(
                    (color.r * fade) / 255,
                    (color.g * fade) / 255,
                    (color.b * fade) / 255);
            }
        }

        // Blur after (next 8 LEDs)
        for (int i = 1; i <= waveSize; i++)
        {
            int idx = wave * waveSize + waveSize - 1 + i;
            if (idx < NUM_LEDS)
            {
                uint8_t fade = map(i, 1, waveSize * 2, 100, 5);
                leds[idx] = CRGB(
                    (color.r * fade) / 255,
                    (color.g * fade) / 255,
                    (color.b * fade) / 255);
            }
        }

        FastLED.show();

        unsigned long startDelay = millis();
        while (millis() - startDelay < 60)
        {
            yield();
        }
    }

    unsigned long startDelay = millis();
    while (millis() - startDelay < waveDelay)
    {
        yield();
    }
}

void clear_leds()
{
    FastLED.clear();
    FastLED.show();
}

// This function creates a bi-directional wave effect on the LEDs.
// It lights up 8 LEDs at a time, moving forward and then backward.
void show_color_bi_wave(RGBColor color)
{
    const int waveSize = 8;
    const unsigned long waveDelay = 50;

    int numWaves = NUM_LEDS / waveSize;

    // Forward run
    for (int wave = 0; wave < numWaves; wave++)
    {
        FastLED.clear();

        for (int i = 0; i < waveSize; i++)
        {
            int idx = wave * waveSize + i;
            if (idx < NUM_LEDS)
            {
                leds[idx] = CRGB(color.r, color.g, color.b);
            }
        }

        FastLED.show();

        // Wait for the wave effect to complete
        unsigned long startDelay = millis();
        while (millis() - startDelay < 50)
        {
            yield();
        }
    }

    // Backward run
    for (int wave = numWaves - 1; wave >= 0; wave--)
    {
        FastLED.clear();

        for (int i = 0; i < waveSize; i++)
        {
            int idx = wave * waveSize + i;
            if (idx < NUM_LEDS)
            {
                leds[idx] = CRGB(color.r, color.g, color.b);
            }
        }

        FastLED.show();

        // Wait for the wave effect to complete
        unsigned long startDelay = millis();
        while (millis() - startDelay < 50)
        {
            yield();
        }
    }

    // Wait for the wave effect to complete
    unsigned long startDelay = millis();
    while (millis() - startDelay < waveDelay)
    {
        yield();
    }
}
