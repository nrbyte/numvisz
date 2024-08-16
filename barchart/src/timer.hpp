#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
{
public:
  using FloatMS = std::chrono::duration<float, std::milli>;

  void start();
  void setTime(FloatMS time);
  FloatMS getInMilliseconds();

private:
  std::chrono::time_point<std::chrono::steady_clock, FloatMS> startPoint;
};

#endif
