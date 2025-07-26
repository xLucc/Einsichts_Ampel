#pragma once
#include <Arduino.h>
#include <array>
enum class TimerState
{
    IDLE,
    RUNNING,
    PAUSED
};

struct RGBColor
{
    uint8_t r, g, b;

    bool operator!=(const RGBColor &other) const
    {
        return r != other.r || g != other.g || b != other.b;
    }
};

class Timer
{
public:
    Timer(uint32_t);
    void start();
    void pause(RGBColor);
    void resume(RGBColor);
    void tick();
    void add(uint32_t, RGBColor);
    void stop();
    void before_stop();

private:
    TimerState state = TimerState::IDLE;
    uint32_t endMillis = 0;
    uint32_t remainingMilllis = 0;
    RGBColor lastLedColor = {0, 0, 0};
    uint32_t max_duration;
    RGBColor calcColor(uint32_t);
};