#include "timer.hpp"

void Timer::start()
{
  startPoint = std::chrono::steady_clock::now();
}

Timer::FloatMS Timer::getInMilliseconds()
{
  return std::chrono::steady_clock::now() - startPoint;
}
