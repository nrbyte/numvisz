#include "barchart.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

// Helper function to generate colors for each row
static void generateColors(std::vector<BarChart::RowState>& rows)
{
    int i = 0;
    int increment = 3 * (255.0 / rows.size());
    for (int r = 0; r <= 255; r += increment)
        for (int g = 0; g <= 255; g += increment)
            for (int b = 0; b <= 255; b += increment)
            {
                if (i < rows.size())
                    rows[i].color =
                        Color{r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
                else
                    return;
                ++i;
            }
}

BarChart::BarChart(const std::string& csvPath, Timer::FloatMS tPC, int bH)
    : parser(csvPath), timePerCategory(tPC), barHeight(bH)
{
    // Go through each row, and put in the starting value, and also
    // get the longest row name, for measurements later
    for (auto& row : parser.getRows())
    {
        rowStates.push_back(
            {row.name, row.values.front(), Color{0.1f, 0.1f, 0.8f, 1.0f}});
        if (row.name.length() > longestRowName.length())
            longestRowName = row.name;
    }

    // Generate the colors
    generateColors(rowStates);
}

void BarChart::update(Timer::FloatMS currentTime, unsigned spacingAboveBars)
{
    int numCategories = getCategories().size();

    // Calculate the current position and next position.
    float currentPosition =
        std::min(currentTime / timePerCategory, float(numCategories - 1));
    int intPrevPosition = std::min(int(currentPosition), numCategories - 2);
    int intNextPosition = std::min(intPrevPosition + 1, numCategories - 1);

    // Update the current category based on the floating point current position
    currentCategory = getCategories()[int(currentPosition)];

    // Update the row's current values
    for (auto& row : parser.getRows())
    {
        // Get the values based on the 2 categories we're inbetween
        long double prevValue = row.values.at(intPrevPosition);
        long double nextValue = row.values.at(intNextPosition);
        long double diff = nextValue - prevValue;
        // The current value is the current value plus a percentage of the
        // difference between this value and the next, to make it look like
        // we're animating toward it.
        long double currentValue =
            prevValue + ((currentPosition - intPrevPosition) * diff);

        // Update the row's current value
        std::find_if(rowStates.begin(), rowStates.end(),
                     [&](const auto& x) { return x.name == row.name; })
            ->value = currentValue;
    }

    // Sort the bars by their values
    std::sort(rowStates.begin(), rowStates.end(),
              [](const auto& x, const auto& y) { return x.value > y.value; });

    highestValue = rowStates.front().value;

    // Update the bar heights
    for (int i = 0; i < rowStates.size(); i++)
    {
        // Calculate the height aim (what the height should be)
        RowState& rs = rowStates[i];
        rs.heightAim = spacingAboveBars + (i * (barHeight + 10));
        if (rs.currentHeight == 0)
            rs.currentHeight = rs.heightAim;
        else
        {
            // Update the heights, move currentHeight towards heightAim
            int diff = rs.heightAim - rs.currentHeight;
            // Make sure numbers like 0.4 and -0.2 are rounded to 1 and
            // -1 respectively, as by default they'd be truncated to 0 - which
            // will cause the difference in height to remain and not shrink
            rs.currentHeight +=
                (diff < 0) ? std::floor(diff / 5.0) : std::ceil(diff / 5.0);
        }
    }
}
