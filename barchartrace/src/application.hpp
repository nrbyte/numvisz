#ifndef APPLICATION_H
#define APPLICATION_H

#include "viszbase/commandlineparser.hpp"
#include "viszbase/gui.hpp"

class Application
{
public:
    explicit Application(Arguments args);

    int run();

private:
    Arguments args;
    GUI gui;
};

#endif
