#include <stdexcept>
#include <iostream>

#include "application.hpp"

int main()
{
  try
  {
    Application app;
    return app.run();
  }
  catch(std::runtime_error e)
  {
    // Prefix all errors with 'ERROR:'
    std::cerr << "ERROR:" << e.what() << std::endl;
    return -1;
  }
}
