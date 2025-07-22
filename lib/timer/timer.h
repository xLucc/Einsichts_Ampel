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
        return r != other.r && g != other.g && b != other.b;
    }
};

class Timer
{
public:
    Timer(uint32_t, CRGB *, uint16_t);
    void start(uint32_t);
    void pause(RGBColor);
    void resume(RGBColor);
    void tick();
    void add(uint32_t, RGBColor);
    void stop();

private:
    TimerState state;
    uint32_t endMillis;
    uint32_t remainingMilllis;
    RGBColor lastLedColor;
    RGBColor calcColor(uint32_t);
};