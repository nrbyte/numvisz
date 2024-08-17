#include <stdexcept>
#include <algorithm>

#include "commandlineparser.hpp"

CommandLineParser::CommandLineParser(int argc, char** argv,
    const std::vector<std::string>& allowedArguments)
{
  // Initialize the initial argument mapping
  // Subsequent argument objects will be made from this
  argMap = std::make_shared<ArgumentMap>();

  // Cycle through arguments
  for (int i = 0; i < argc; i++)
  {
    const char* arg = argv[i];
    // If an arg has a '-' infront, it is setting an option
    // Find out what that argument is and put it in the map
    if (arg[0] == '-')
    {
      // Check there is a next argument to use as the value for this option
      if (i == (argc-1))
        throw std::runtime_error(
            "Couldn't parse arguments, argument specified with no value");

      // Check this argument is supported
      if (std::find(allowedArguments.begin(), allowedArguments.end(), arg)
          == allowedArguments.end())
        throw std::runtime_error(
            std::string("Invalid argument: ") + arg);

      // Map the argument and value
      (*argMap)[arg] = argv[i+1];
    }
  }
}

// Construct a new arguments object, with a safe ptr to the original arg map
Arguments CommandLineParser::getArguments()
{
  Arguments args(argMap);
  return args;
}

const std::string& Arguments::get
  (const std::string& option, const std::string& defaultValue) const
{
  // Try find a mapping
  auto find = argMap->find(option);
  // If no mapping exists, return user-specified default value
  if (find == argMap->end())
    return defaultValue;
  // Otherwise, return the mapped value
  return find->second;
}

int Arguments::getInt(const std::string& option, int defaultValue) const
{
  const std::string& strVal = get(option);
  if (strVal == Arguments::NotSet)
    return defaultValue;
  return std::stoi(strVal);
}
