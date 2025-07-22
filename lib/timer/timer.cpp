#include <timer.h>
#include <led.h>

// Timer class manages a countdown timer and updates LED colors based on remaining time.
class Timer
{
public:
    // Constructor initializes timer with maximum duration and LED array.
    Timer::Timer(uint32_t max_duration, CRGB *leds, uint16_t numLeds)
    {
        this->max_duration = max_duration;
    }

    // Starts the timer for a given duration and updates LEDs.
    void start(uint32_t duration)
    {
        state = TimerState::RUNNING;
        endMillis = millis() + duration;
        RGBColor color = calcColor(duration);
        RGBColor identifier = {0, 255, 0};
        show_color_wave(identifier);
        show_color(color);
    }

    // Pauses the timer, saving remaining time.
    void pause(RGBColor color)
    {
        if (state == TimerState::RUNNING)
        {
            remainingMilllis = endMillis - millis();
            state = TimerState::PAUSED;
            show_color_identifier(lastLedColor, color);
        }
    }

    // Resumes the timer from paused state.
    void resume(RGBColor color)
    {
        if (state == TimerState::PAUSED)
        {
            endMillis = millis() + remainingMilllis;
            state = TimerState::RUNNING;
            show_color_identifier(lastLedColor, color);
        }
    }

    // Adds extra time to the timer.
    void add(uint32_t extra, RGBColor color)
    {
        endMillis += extra;
        if (state == TimerState::PAUSED)
        {
            remainingMilllis += extra;
        }
        show_color_identifier(lastLedColor, color);
    }

    // Updates timer state and LED color; should be called regularly.
    void tick()
    {
        if (state != TimerState::RUNNING)
        {
            return;
        }

        uint32_t left = (endMillis > millis()) ? (endMillis - millis()) : 0;

        RGBColor newColor = calcColor(left);

        if (newColor != lastLedColor)
        {
            lastLedColor = newColor;
            show_color(lastLedColor);
        }

        if (left == 0)
        {
            state = TimerState::IDLE;
        }
    }

    void stop()
    {
        state = TimerState::IDLE;
        endMillis = 0;
        remainingMilllis = 0;
        show_color_wave({255, 0, 0});
        lastLedColor = {0, 0, 0};
        show_color(lastLedColor);
    }

private:
    // Current state of the timer.
    TimerState state = TimerState::IDLE;
    // Maximum duration for the timer.
    uint32_t max_duration;
    // Time when the timer should end.
    uint32_t endMillis = 0;
    // Remaining time when paused.
    uint32_t remainingMilllis = 0;
    // Last color shown on LEDs.
    RGBColor lastLedColor = {0, 0, 0};

    // Calculates LED color based on remaining time.
    RGBColor calcColor(uint32_t left)
    {
        RGBColor color;
        float ratio = min(1.0f, left / (float)max_duration);
        color.r = 255 * (1.0f - ratio);
        color.g = 255 * ratio;
        color.b = 0;

        return color;
    }
};