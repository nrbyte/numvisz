#include <iostream>

#include "application.hpp"
#include "math.hpp"

int main()
{
  // Test it works
  math::Matrix<4, 4> scale;
  math::setScale(scale, 3, 4, 3);
  std::cout << "Scale: " << scale << '\n';

  math::Matrix<4, 4> tran;
  math::setTranslate(tran, 1, 1, 1);
  std::cout << "Translate: " << tran << '\n';

  math::Matrix<4, 1> test;
  test.put(0, 0, 1);
  test.put(1, 0, 2);
  test.put(2, 0, 3);
  test.put(3, 0, 1);
  std::cout << "x = " << test << '\n';

  math::Matrix<4, 1> result = tran * scale * test;
  std::cout << result << std::endl;

  return 0;
  
  Application app;
  return app.run();
}
