#ifndef CSVPARSER_HPP
#define CSVPARSER_HPP

#include <string>
#include <vector>

struct Row
{
  std::string name;
  std::vector<long double> values;
};

class CsvParser
{
public:
  CsvParser(const std::string& fileName);

  long long getValue(int row, int position) {
    return rows[row].values[position];
  }
  std::string& getCategory(int position) {
    return categories[position];
  }
  std::string& getName() {
    return name;
  }

private:
  std::string name;

  std::vector<std::string> categories;
  std::vector<Row> rows;
};

#endif
