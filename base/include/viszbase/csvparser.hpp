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
    explicit CsvParser(const std::string& fileName);

    const std::vector<std::string>& getCategories() const { return categories; }
    const std::vector<Row>& getRows() const { return rows; }
    std::string& getName() { return name; }

private:
    std::string name;

    std::vector<std::string> categories;
    std::vector<Row> rows;
};

#endif
