#ifndef LINECHART_HPP
#define LINECHART_HPP

#include "viszbase/csvparser.hpp"
#include "viszbase/color.hpp"
#include "viszbase/timer.hpp"
#include "viszbase/linerenderer.hpp"

class LineChart
{
public:
    LineChart(const std::string& csvName, Timer::FloatMS timePerCategory,
              int lineThickness);

    void update(Timer::FloatMS currentTime);
    float getLowestValue() { return lowestValue; }
    float getHighestValue() { return highestValue; }

    const std::string& getName() { return parser.getName(); }
    const std::vector<Row>& getRows() { return parser.getRows(); }
    const std::string& getLongestRowName() { return longestRowName; }
    float getCurrentPosition() { return currentPosition; }
    int getNextPosition() { return intNextPosition; }
    Timer::FloatMS getCurrentTime() { return currentTime; }

    struct Line
    {
        std::string name;
        Color color;
        LineRenderer renderer;
        float currentValue;
    };
    const std::vector<std::string>& getCategories()
    {
        return parser.getCategories();
    }
    int getNumCategories() { return numCategories; }
    std::vector<Line>& getLineStates() { return lineStates; }
    const std::string& getCurrentCategory() { return currentCategory; }

private:
    Timer::FloatMS timePerCategory;
    CsvParser parser;
    int lineThickness;
    int numCategories;

    std::vector<Line> lineStates;
    float currentPosition;
    std::string currentCategory;
    std::string longestRowName;

    float highestValue = std::numeric_limits<float>().min(),
          lowestValue = std::numeric_limits<float>().max(), height = 0.0f;
    int intCurrentPosition = 0, intNextPosition = 0;
    Timer::FloatMS currentTime;
};

#endif // LINECHART_HPP
