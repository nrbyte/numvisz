#include <iostream>

#include "application.hpp"
#include "math.hpp"

int main()
{
  // Test it works
  math::Matrix<4, 4> iden;
  math::setIdentity(iden);
  iden.put(2, 2, 3);

  math::Matrix<4, 1> test;
  test.put(0, 0, 1);
  test.put(1, 0, 2);
  test.put(2, 0, 3);
  test.put(3, 0, 4);


  math::Matrix<4, 1> result = iden * test;
  std::cout << result << std::endl;

  return 0;
  
  Application app;
  return app.run();
}
