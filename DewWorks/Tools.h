#ifndef _Tools_h
#define _Tools_h

class Timer
{
public:
    Timer(unsigned long intervall);
    bool ShouldRun(unsigned long now);
private:
    unsigned long intervallMs;
    unsigned long lastRun = 0;
};

#endif