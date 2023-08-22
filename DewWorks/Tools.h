#ifndef _Tools_h
#define _Tools_h

class Timer
{
public:
    explicit Timer(unsigned long intervallMs);
    bool shouldRun(unsigned long now);
private:
    unsigned long intervallMs;
    unsigned long lastRun = 0;
};

#endif