#include "timer.hpp"

void Timer::start()
{
  startPoint = std::chrono::steady_clock::now();
}

void Timer::setTime(FloatMS time)
{
  startPoint = std::chrono::steady_clock::now();
  startPoint -= time;
}

Timer::FloatMS Timer::getInMilliseconds()
{
  return std::chrono::steady_clock::now() - startPoint;
}
