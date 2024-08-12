#ifndef APPLICATION_H
#define APPLICATION_H

#include "commandlineparser.hpp"

#include "gui.hpp"

class Application
{
public:
  Application(Arguments args);

  int run();

private:
  Arguments args;
  GUI gui;
};

#endif
