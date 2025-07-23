#include <timer.h>
#include <led.h>

// Constructor initializes timer with maximum duration and LED array.
Timer::Timer(uint32_t max_duration)
{
    this->max_duration = max_duration;
}

// Starts the timer for a given duration and updates LEDs.
void Timer::start()
{
    state = TimerState::RUNNING;
    endMillis = millis() + max_duration;
    RGBColor color = calcColor(max_duration);
    RGBColor identifier = {0, 255, 0};
    show_color_wave(identifier);
    show_color(color);
}

// Pauses the timer, saving remaining time.
void Timer::pause(RGBColor color)
{
    if (state == TimerState::RUNNING)
    {
        remainingMilllis = endMillis - millis();
        state = TimerState::PAUSED;
        show_color_identifier(lastLedColor, color);
    }
}

// Resumes the timer from paused state.
void Timer::resume(RGBColor color)
{
    if (state == TimerState::PAUSED)
    {
        endMillis = millis() + remainingMilllis;
        state = TimerState::RUNNING;
        show_color_identifier(lastLedColor, color);
    }
}

// Adds extra time to the timer.
void Timer::add(uint32_t extra, RGBColor color)
{
    endMillis += extra;
    if (state == TimerState::PAUSED)
    {
        remainingMilllis += extra;
    }
    show_color_identifier(lastLedColor, color);
}

// Updates timer state and LED color; should be called regularly.
void Timer::tick()
{
    if (state != TimerState::RUNNING)
    {
        return;
    }

    uint32_t left = (endMillis > millis()) ? (endMillis - millis()) : 0;

    RGBColor newColor = calcColor(left);

    if (newColor.r != lastLedColor.r ||
        newColor.g != lastLedColor.g ||
        newColor.b != lastLedColor.b)
    {
        lastLedColor = newColor;
        show_color(lastLedColor);
    }

    if (left == 0)
    {
        stop();
        state = TimerState::IDLE;
    }
}

void Timer::stop()
{
    state = TimerState::IDLE;
    endMillis = 0;
    remainingMilllis = 0;
    show_color_wave({255, 0, 0});
    lastLedColor = {0, 0, 0};
    show_color(lastLedColor);
}

// Calculates LED color based on remaining time.
RGBColor Timer::calcColor(uint32_t left)
{
    RGBColor color;
    float ratio = min(1.0f, left / (float)max_duration);
    color.r = 255 * (1.0f - ratio);
    color.g = 255 * ratio;
    color.b = 0;

    return color;
}
