#include <stdexcept>
#include <cmath>

#include "application.hpp"

#include "viszbase/linerenderer.hpp"
#include "viszbase/renderer.hpp"
#include "viszbase/fontrenderer.hpp"
#include "viszbase/math.hpp"
#include "viszbase/csvparser.hpp"
#include "viszbase/timer.hpp"

Application::Application(Arguments arg) : args{arg} {}

struct Line
{
    std::string name;
    Color color;
    LineRenderer renderer;
    float currentValue;
};

// Helper function to generate colors for each row
static void generateColors(std::vector<Line>& lines)
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

int Application::run()
{
    // Setup a window, MUST NOT CALL ANY OPENGL BEFORE THIS
    gui.setup(800, 600, "Visualization");

    Timer timer;
    Renderer renderer;
    FontRenderer fontRendererSmall;
    FontRenderer fontRenderer;
    FontRenderer fontRendererLarge;
    math::Matrix<4, 4> proj;

    // Parse the provided CSV
    const std::string& fileName = args.get("-csv");
    if (fileName == Arguments::NotSet)
        throw std::runtime_error("CSV file name not provided!");
    CsvParser csv(fileName);
    int numCategories = csv.getCategories().size();

    // Get time per category from arguments if set, otherwise use
    // sensible default
    Timer::FloatMS timePerCategory{args.getInt("-timepercategory", 100)};

    // Store number of decimal places for drawing numbers
    int numOfDecimalPlaces = args.getInt("-decimalplaces", 0);

    // Get line thickness from arguments if set, otherwise use
    // sensible default
    float lineThickness = args.getInt("-linethickness", 10);
    lineThickness /= 1000;

    // Load the provided font file
    const std::string& fontName = args.get("-font");
    if (fontName == Arguments::NotSet)
        throw std::runtime_error("Font file not provided");
    fontRendererSmall.loadFont(fontName, 10);
    fontRenderer.loadFont(fontName, 16);
    fontRendererLarge.loadFont(fontName, 24);

    // Padding and Spacing values
    struct
    {
        unsigned beforeLines = 30;
        unsigned aboveLines = 30;
        unsigned belowLines = 30;
        unsigned afterLines = 30;
    } Paddings;
    struct
    {
        unsigned beforeLines;
        unsigned aboveLines;
        unsigned belowLines;
        unsigned afterLines;
    } Spacings;
    Spacings.beforeLines = Paddings.beforeLines;
    Spacings.aboveLines =
        Paddings.aboveLines + fontRendererLarge.getFontHeight();
    Spacings.belowLines = Paddings.belowLines + fontRenderer.getFontHeight();
    Spacings.afterLines = Paddings.afterLines;

    // Go through each line, and load in the values
    std::vector<Line> lines;
    std::string longestRowName = "";
    for (auto& row : csv.getRows())
    {
        LineRendererBuilder builder;

        float x = 0.0f;
        for (const auto& value : row.values)
        {
            builder.addPoint(x, value);
            x += timePerCategory.count();
        }
        lines.push_back(
            {row.name, Color{0.5f, 0.5f, 0.5f, 1.0f}, builder.build(), 0.0f});

        // Update longest row name
        if (row.name.size() > longestRowName.size())
            longestRowName = row.name;
    }

    // Update spacing based on longest row name
    float newSpacingBeforeLines;
    float newSpacingAfterLines =
        fontRenderer.getWidthOfMsg(longestRowName) + Paddings.afterLines;
    if (Spacings.afterLines < newSpacingAfterLines)
        Spacings.afterLines = newSpacingAfterLines;

    // Generate colours
    generateColors(lines);

    math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);

    float highestValue = std::numeric_limits<float>().min(),
          lowestValue = std::numeric_limits<float>().max(), height = 0.0f;
    // Text Panel = the row name and value that appears next to the line
    float textPanelHeight = fontRenderer.getFontHeight() * 1.5;
    // Keep track of current position
    float currentPosition;
    int intCurrentPosition, intNextPosition;
    Timer::FloatMS currentTime;
    // Start the timer and start drawing
    timer.start();
    while (gui.windowStillOpen())
    {
        gui.clearScreen(Color{1.0f, 1.0f, 1.0f, 1.0f});

        currentTime = std::min(timer.getInMilliseconds(),
                               (numCategories - 1) * timePerCategory);

        currentPosition = currentTime / timePerCategory;
        intCurrentPosition = std::min(int(currentPosition), numCategories - 2);
        intNextPosition = std::min(intCurrentPosition + 1, numCategories - 1);

        // Update current values
        float prevValue, nextValue, diff;
        for (auto& row : csv.getRows())
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
            std::find_if(lines.begin(), lines.end(),
                         [&](const auto& l) { return l.name == row.name; })
                ->currentValue = currentValue;
        }
        std::sort(lines.begin(), lines.end(), [](const auto& x, const auto& y)
                  { return x.currentValue > y.currentValue; });

        // Update height if necessary
        if (lines.front().currentValue > highestValue)
            highestValue = lines.front().currentValue;
        if (lines.back().currentValue < lowestValue)
            lowestValue = lines.back().currentValue;
        height = highestValue - lowestValue;

        // Update spacing if necessary (the lowest number can be longer than the
        // largest number so calculate the max width out of both)
        newSpacingAfterLines =
            Paddings.afterLines +
            std::max(fontRenderer.getWidthOfLongDouble(
                         lines.front().currentValue, numOfDecimalPlaces),
                     fontRenderer.getWidthOfLongDouble(
                         lines.back().currentValue, numOfDecimalPlaces));
        newSpacingBeforeLines =
            Paddings.afterLines +
            std::max(fontRendererSmall.getWidthOfLongDouble(
                         lines.front().currentValue, numOfDecimalPlaces),
                     fontRendererSmall.getWidthOfLongDouble(
                         lines.back().currentValue, numOfDecimalPlaces));

        Spacings.afterLines =
            std::max((float)Spacings.afterLines, newSpacingAfterLines);
        Spacings.beforeLines =
            std::max((float)Spacings.beforeLines, newSpacingBeforeLines);

        // Draw categories along the bottom along with lines
        // Calculate what the interval should be between categories to ensure it
        // isn't crowded and categories' names don't overlap
        int interval = 1;
        while (true)
        {
            // If this interval is larger than the amount of categories,
            // subtract 1 and don't increase further
            if (interval > numCategories - 1)
            {
                --interval;
                break;
            }
            // Calculate the required X position of the next category to be
            // displayed
            float requiredX =
                (interval / currentPosition) *
                (gui.width - Spacings.beforeLines - Spacings.afterLines);

            // If the next category's X position leaves enough room from the
            // first category, then this interval is fine
            if (requiredX >
                fontRenderer.getWidthOfMsg(csv.getCategories()[0]) * 2)
                break;

            // Otherwise, increase the interval and try again
            interval++;
        }
        for (int i = 0; i <= intNextPosition; i += interval)
        {
            float percentAcrossLine = (i / currentPosition);
            if (percentAcrossLine > 1)
                continue;

            float x = Spacings.beforeLines +
                      percentAcrossLine * (gui.width - Spacings.beforeLines -
                                           Spacings.afterLines);

            fontRenderer.drawMsg(
                x, gui.height - Spacings.belowLines + Paddings.belowLines * 0.2,
                csv.getCategories()[i], proj);

            renderer.drawBox(x, Spacings.aboveLines, x + 2,
                             gui.height - Spacings.belowLines,
                             Color{0, 0, 0, 0.1f}, proj);
        }

        // Set projection
        math::setOrtho(proj, highestValue + (height * (lineThickness / 2)),
                       currentTime.count(),
                       lowestValue - (height * (lineThickness / 2)), 0, -0.1f,
                       -100.0f);
        // Update viewport to leave space around the lines
        gui.setViewport(Spacings.beforeLines, Spacings.belowLines,
                        gui.width - Spacings.afterLines - Spacings.beforeLines,
                        gui.height - Spacings.aboveLines - Spacings.belowLines);
        // Draw the lines in the order they appear in the CSV
        float aspectRatio = float(gui.width) / gui.height;
        for (auto& row : csv.getRows())
        {
            auto line =
                std::find_if(lines.begin(), lines.end(),
                             [&](const auto& l) { return l.name == row.name; });
            line->renderer.draw(line->color, aspectRatio, lineThickness, proj);
        }

        // Reset viewport and projection
        gui.setViewport(0, 0, gui.width, gui.height);
        math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);

        // Draw frame around lines
        // Draw line along side
        renderer.drawBox(
            Spacings.beforeLines, Spacings.aboveLines, Spacings.beforeLines + 1,
            gui.height - Spacings.belowLines, Color{0, 0, 0, 1}, proj);
        // Draw line along top
        renderer.drawBox(Spacings.beforeLines, Spacings.aboveLines,
                         gui.width - Spacings.afterLines,
                         Spacings.aboveLines + 1, Color{0, 0, 0, 1}, proj);
        // Draw line along bottom
        renderer.drawBox(Spacings.beforeLines, gui.height - Spacings.belowLines,
                         gui.width - Spacings.afterLines,
                         gui.height - Spacings.belowLines + 1,
                         Color{0, 0, 0, 1}, proj);

        // Draw text
        // Draw title
        fontRendererLarge.drawMsg(Spacings.beforeLines,
                                  Paddings.aboveLines / 2.0, csv.getName(),
                                  proj);
        // Draw row names and values next to lines
        float nextAvailableY = 0.0f;
        for (auto& line : lines)
        {
            float textY =
                Spacings.aboveLines - fontRenderer.getFontHeight() * 0.5 +
                (1 - (line.currentValue - lowestValue) / height) *
                    (gui.height - Spacings.aboveLines - Spacings.belowLines);
            if (textY < nextAvailableY)
                textY = nextAvailableY;

            fontRenderer.drawMsg(gui.width - Spacings.afterLines +
                                     Paddings.afterLines * 0.2,
                                 textY, line.name, proj);
            fontRenderer.drawLongDouble(
                gui.width - Spacings.afterLines + Paddings.afterLines * 0.2,
                textY + fontRenderer.getFontHeight() * 0.8, line.currentValue,
                numOfDecimalPlaces, proj);

            nextAvailableY = textY + textPanelHeight;
        }

        // Draw the highest and lowest values along the left side
        fontRendererSmall.drawLongDouble(
            Spacings.beforeLines - Paddings.beforeLines * 0.5 -
                fontRendererSmall.getWidthOfLongDouble(highestValue,
                                                       numOfDecimalPlaces),
            Spacings.aboveLines - (fontRendererSmall.getFontHeight() * 0.5),
            highestValue, numOfDecimalPlaces, proj);
        fontRendererSmall.drawLongDouble(
            Spacings.beforeLines - Paddings.beforeLines * 0.5 -
                fontRendererSmall.getWidthOfLongDouble(lowestValue,
                                                       numOfDecimalPlaces),
            gui.height - Spacings.belowLines -
                (fontRendererSmall.getFontHeight() * 0.5),
            lowestValue, numOfDecimalPlaces, proj);

        // Advance to the next frame
        gui.nextFrame();
    }
    return 0;
}
