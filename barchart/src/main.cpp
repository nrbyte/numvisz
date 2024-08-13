#include <stdexcept>
#include <iostream>

#include "application.hpp"
#include "commandlineparser.hpp"

int main(int argc, char** argv)
{
  try
  {
    // Parse arguments
    CommandLineParser parser(argc, argv, {"-csv", "-barheight", "-font"});
    // Start application with those parsed arguments
    Application app(parser.getArguments());
    return app.run();
  }
  catch(std::runtime_error e)
  {
    // Prefix all errors with 'ERROR:'
    std::cerr << "ERROR:" << e.what() << std::endl;
    return -1;
  }
}
