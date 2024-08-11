#include "csvparser.hpp"

#include <fstream>
#include <algorithm>

CsvParser::CsvParser(const std::string& fileName)
{
  std::ifstream csvFile(fileName);
  std::string line;

  if (!csvFile.is_open())
    throw std::runtime_error("Failed to open CSV file");

  // Read in the categories row
  std::getline(csvFile, line);

  // Read in the title
  auto it = std::find(line.begin(), line.end(), ',');
  name = std::string(line.begin(), it);
  // Read in the categories
  auto prevIt = it;
  while (it != line.end())
  {
    it = std::find(prevIt+1, line.end(), ',');
    std::string category(prevIt+1, it);
    if (!category.empty())
      categories.push_back(std::string(prevIt+1, it));

    prevIt = it;
  }

  // Read in each row
  while (std::getline(csvFile, line))
  {
    Row r;
    
    // Read in the title
    auto it = std::find(line.begin(), line.end(), ',');
    r.name = std::string(line.begin(), it);

    // Read in the values
    auto prevIt = it;
    while (it != line.end())
    {
      it = std::find(prevIt+1, line.end(), ',');
      std::string value(prevIt+1, it);
      if (!value.empty())
        r.values.push_back(std::stold(value));
      else
        // If the value is blank, i.e. 2 adjacent commas, enter 0
        r.values.push_back(0);

      prevIt = it;
    }

    rows.push_back(std::move(r));
  }
}
