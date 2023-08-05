#include "Tools.h"

Timer::Timer(unsigned long intervallMs)
{
    this->intervallMs = intervallMs;
    this->lastRun = 0;
}

bool Timer::ShouldRun(unsigned long now)
{
    if (now - this->lastRun > this->intervallMs)
    {
        this->lastRun = now;
        return true;
    }
    return false;
}
