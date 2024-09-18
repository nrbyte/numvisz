#ifndef BAR_CHART_HPP
#define BAR_CHART_HPP

#include "viszbase/color.hpp"
#include "viszbase/csvparser.hpp"
#include "viszbase/timer.hpp"

class BarChart
{
public:
    BarChart(const std::string& csvPath, Timer::FloatMS timePerCategory,
             int barHeight);
    void update(Timer::FloatMS currentTime, unsigned spcaingAboveBars);

    const std::string& getName() { return parser.getName(); }
    const std::string& getLongestRowName() { return longestRowName; }
    const long double& getHighestValue() { return highestValue; }
    const std::vector<std::string>& getCategories()
    {
        return parser.getCategories();
    }

    struct RowState
    {
        std::string name;
        long double value;
        Color color;
        // To animate the bar moving positions
        int currentHeight;
        int heightAim;
    };
    const std::vector<RowState>& getRowStates() { return rowStates; }

    const std::string& getCurrentCategory() { return currentCategory; }

private:
    std::vector<RowState> rowStates;
    std::string longestRowName;
    long double highestValue;
    std::string currentCategory;

    CsvParser parser;
    Timer::FloatMS timePerCategory;
    int barHeight;
};

#endif
