#include "viszbase/csvparser.hpp"

#include <fstream>
#include <algorithm>
#include <locale>
#include <sstream>

CsvParser::CsvParser(const std::string& fileName)
{
    std::ifstream csvFile(fileName);
    std::string line;

    std::istringstream is;
    is.imbue(std::locale(""));

    if (!csvFile.is_open())
        throw std::runtime_error("Failed to open CSV file");

    try
    {
        // Read in the categories row
        std::getline(csvFile, line);

        // Read in the title
        auto it = std::find(line.begin(), line.end(), ',');
        name = std::string(line.begin(), it);
        // Read in the categories
        auto prevIt = it;
        while (it != line.end())
        {
            it = std::find(prevIt + 1, line.end(), ',');
            std::string category(prevIt + 1, it);
            if (!category.empty())
                categories.push_back(std::string(prevIt + 1, it));
            else
                throw std::exception();

            prevIt = it;
        }

        // Read in each row
        while (std::getline(csvFile, line))
        {
            Row r;

            // Read in the title
            auto it = std::find(line.begin(), line.end(), ',');
            r.name = std::string(line.begin(), it);

            // If the row's first column is empty, skip the row
            if (r.name.empty())
                continue;

            // Read in the values
            auto prevIt = it;
            while (it != line.end())
            {
                // If theres a quotation mark present, look for the closing
                // quotation mark, not a comma, and then add 1 to get to the
                // comma after it (or line.end())
                if ((prevIt + 1) != line.end() && *(prevIt + 1) == '"')
                    it = 1 + std::find(prevIt + 2, line.end(), '"');
                else
                    it = std::find(prevIt + 1, line.end(), ',');

                std::string value(prevIt + 1, it);
                if (!value.empty())
                {
                    if (value[0] == '"')
                    {
                        value.erase(value.begin());
                        value.erase(value.end() - 1);
                    }

                    is.clear();
                    is.str(value);
                    long double ld;
                    is >> ld;
                    r.values.push_back(ld);
                }
                else
                {
                    if (r.values.empty())
                        // If the value is blank, i.e. 2 adjacent commas, enter
                        // 0
                        r.values.push_back(0);
                    else
                        r.values.push_back(r.values.back());
                }

                prevIt = it;
            }
            rows.push_back(std::move(r));
        }
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(
            "Failed to parse/understand CSV file, are you sure its valid!");
    }
}
