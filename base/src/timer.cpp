#include "viszbase/timer.hpp"

void Timer::start() { startPoint = std::chrono::steady_clock::now(); }

void Timer::setTime(FloatMS time)
{
    // If stopped, update the time it was stopped at
    if (stopped)
        timeStoppedAt = time;
    // Otherwise, set the actual time
    else
    {
        startPoint = std::chrono::steady_clock::now();
        startPoint -=
            std::chrono::duration_cast<std::chrono::nanoseconds>(time);
    }
}

Timer::FloatMS Timer::getInMilliseconds()
{
    // If frozen, return the time it was stopped at
    if (stopped)
        return timeStoppedAt;
    else
        return std::chrono::steady_clock::now() - startPoint;
}

void Timer::stop()
{
    // Set timer to stopped at its current time
    timeStoppedAt = getInMilliseconds();
    stopped = true;
}

void Timer::resume()
{
    // Set the timer to the time it was stopped at, and change it to unfrozen
    if (stopped)
    {
        // First unstop the timer
        stopped = false;
        // Then set its time to the time stopped at, so that it actually sets
        // the current time of the timer, not the stopped time (i.e. timer must
        // be set to unstopped first)
        setTime(timeStoppedAt);
    }
}
