#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
{
public:
  void start();

  using FloatMS = std::chrono::duration<float, std::milli>;
  FloatMS getInMilliseconds();

private:
  std::chrono::time_point<std::chrono::steady_clock> startPoint;
};

#endif
