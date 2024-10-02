#include "linechart.hpp"

// Helper function to generate colors for each row
static void generateColors(std::vector<LineChart::Line>& lines)
{
    int i = 0;
    int increment = 3 * (255.0 / lines.size());
    for (int r = 0; r <= 255; r += increment)
    {
        for (int g = 0; g <= 255; g += increment)
        {
            for (int b = 0; b <= 255; b += increment)
            {
                if (i < lines.size())
                    lines[i].color =
                        Color{r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
                else
                    return;
                ++i;
            }
        }
    }
}

LineChart::LineChart(const std::string& csvName, Timer::FloatMS tPC, int lT)
    : parser(csvName), timePerCategory(tPC), lineThickness(lT)
{
    numCategories = parser.getCategories().size();

    // Go through each line, and load in the values
    for (auto& row : parser.getRows())
    {
        LineRendererBuilder builder;

        float x = 0.0f;
        for (const auto& value : row.values)
        {
            builder.addPoint(x, value);
            x += timePerCategory.count();
        }
        lineStates.push_back(
            {row.name, Color{0.1f, 0.1f, 0.8f, 1.0f}, builder.build(), 0.0f});

        // Update longest row name
        if (row.name.size() > longestRowName.size())
            longestRowName = row.name;
    }

    // Generate colours
    generateColors(lineStates);
}

void LineChart::update(Timer::FloatMS time)
{
    currentTime = std::min(time, (numCategories - 1) * timePerCategory);

    currentPosition = currentTime / timePerCategory;
    intCurrentPosition = std::min(int(currentPosition), numCategories - 2);
    intNextPosition = std::min(intCurrentPosition + 1, numCategories - 1);

    // Update current values
    float prevValue, nextValue, diff;
    for (auto& row : parser.getRows())
    {
        prevValue = row.values.at(intCurrentPosition);
        nextValue = row.values.at(intNextPosition);

        float diff = nextValue - prevValue;
        // The current value is the previous value plus a fraction of the
        // difference to the next value, to give the look that we're moving
        // to the next value gradually as time progresses.
        float currentValue =
            prevValue + ((currentPosition - intCurrentPosition) * diff);

        // Update the current value in the lines vector
        std::find_if(lineStates.begin(), lineStates.end(),
                     [&](const auto& l) { return l.name == row.name; })
            ->currentValue = currentValue;
    }
    std::sort(lineStates.begin(), lineStates.end(),
              [](const auto& x, const auto& y)
              { return x.currentValue > y.currentValue; });

    // Update highest and lowest values
    highestValue = std::max(highestValue, lineStates.front().currentValue);
    lowestValue = std::min(lowestValue, lineStates.back().currentValue);
}
