#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

using ArgumentMap = std::unordered_map<std::string, std::string>;

class CommandLineParser;

// This class' objects are passed around
class Arguments
{
private:
  // Allow only the parser to construct objects of this type
  Arguments(std::shared_ptr<ArgumentMap> map)
    : argMap {map}
  {}
  friend class CommandLineParser;

public:
  // This value can never be an argument on its own because there is no
  // quotes around the NOT SET, therefore, the space would split it into
  // 2 seperate arguments
  constexpr static const char* NotSet = "\0";

  const std::string& get
    (const std::string& option,
     const std::string& defaultValue = Arguments::NotSet) const;

  int getInt(const std::string& option, int defaultValue) const;

  std::shared_ptr<ArgumentMap> argMap;
};

class CommandLineParser
{
public:
  CommandLineParser(int argc, char** argv,
      const std::vector<std::string>& allowedArguments);

  Arguments getArguments();

private:
  std::shared_ptr<ArgumentMap> argMap;
};

#endif
