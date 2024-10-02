#include <stdexcept>
#include <cmath>

#include "application.hpp"
#include "linechart.hpp"

#include "viszbase/linerenderer.hpp"
#include "viszbase/renderer.hpp"
#include "viszbase/fontrenderer.hpp"
#include "viszbase/math.hpp"
#include "viszbase/csvparser.hpp"
#include "viszbase/timer.hpp"

Application::Application(Arguments arg) : args{arg} {}

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

    // Setup line chart race
    LineChart lineChart(fileName, timePerCategory, lineThickness);

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

    // Update spacing based on longest row name
    float newSpacingBeforeLines;
    float newSpacingAfterLines =
        fontRenderer.getWidthOfMsg(lineChart.getLongestRowName()) +
        Paddings.afterLines;
    if (Spacings.afterLines < newSpacingAfterLines)
        Spacings.afterLines = newSpacingAfterLines;

    math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);

    float highestValue, lowestValue, height = 0.0f;
    // Text Panel = the row name and value that appears next to the line
    float textPanelHeight = fontRenderer.getFontHeight() * 1.5;
    // Start the timer and start drawing
    timer.start();

    while (gui.windowStillOpen())
    {
        gui.clearScreen(Color{1.0f, 1.0f, 1.0f, 1.0f});

        // Update state of line chart
        lineChart.update(timer.getInMilliseconds());

        // Update height
        lowestValue = lineChart.getLowestValue();
        highestValue = lineChart.getHighestValue();
        height = highestValue - lowestValue;

        // Update spacing if necessary (the lowest number can be longer than the
        // largest number so calculate the max width out of both)
        newSpacingAfterLines = Paddings.afterLines +
                               std::max(fontRenderer.getWidthOfLongDouble(
                                            highestValue, numOfDecimalPlaces),
                                        fontRenderer.getWidthOfLongDouble(
                                            lowestValue, numOfDecimalPlaces));
        newSpacingBeforeLines = Paddings.beforeLines +
                                std::max(fontRendererSmall.getWidthOfLongDouble(
                                             highestValue, numOfDecimalPlaces),
                                         fontRendererSmall.getWidthOfLongDouble(
                                             lowestValue, numOfDecimalPlaces));
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
            if (interval > lineChart.getNumCategories() - 1)
            {
                --interval;
                break;
            }
            // Calculate the required X position of the next category to be
            // displayed
            float requiredX =
                (interval / lineChart.getCurrentPosition()) *
                (gui.width - Spacings.beforeLines - Spacings.afterLines);

            // If the next category's X position leaves enough room from the
            // first category, then this interval is fine
            if (requiredX >
                fontRenderer.getWidthOfMsg(lineChart.getCategories()[0]) * 2)
                break;

            // Otherwise, increase the interval and try again
            interval++;
        }
        for (int i = 0; i <= lineChart.getNextPosition(); i += interval)
        {
            float percentAcrossLine = (i / lineChart.getCurrentPosition());
            if (percentAcrossLine > 1)
                continue;

            float x = Spacings.beforeLines +
                      percentAcrossLine * (gui.width - Spacings.beforeLines -
                                           Spacings.afterLines);

            fontRenderer.drawMsg(
                x, gui.height - Spacings.belowLines + Paddings.belowLines * 0.2,
                lineChart.getCategories()[i], proj);

            renderer.drawBox(x, Spacings.aboveLines, x + 2,
                             gui.height - Spacings.belowLines,
                             Color{0, 0, 0, 0.1f}, proj);
        }

        // Set projection
        math::setOrtho(proj, highestValue + (height * (lineThickness / 2)),
                       lineChart.getCurrentTime().count(),
                       lowestValue - (height * (lineThickness / 2)), 0, -0.1f,
                       -100.0f);
        // Update viewport to leave space around the lines
        gui.setViewport(Spacings.beforeLines, Spacings.belowLines,
                        gui.width - Spacings.afterLines - Spacings.beforeLines,
                        gui.height - Spacings.aboveLines - Spacings.belowLines);
        // Draw the lines in the order they appear in the CSV
        float aspectRatio = float(gui.width) / gui.height;
        for (auto& row : lineChart.getRows())
        {
            auto line =
                std::find_if(lineChart.getLineStates().begin(),
                             lineChart.getLineStates().end(),
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
                                  Paddings.aboveLines / 2.0,
                                  lineChart.getName(), proj);
        // Draw row names and values next to lines
        float nextAvailableY = 0.0f;
        for (auto& line : lineChart.getLineStates())
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

        // If highest value > 0, and lowest value < 0, draw the 0
        if (highestValue > 0 && lowestValue < 0)
        {
            fontRendererSmall.drawLongDouble(
                Spacings.beforeLines - Paddings.beforeLines * 0.5 -
                    fontRendererSmall.getWidthOfLongDouble(0, 0),
                Spacings.aboveLines - fontRendererSmall.getFontHeight() * 0.5 +
                    (1 - (-lowestValue / height)) *
                        (gui.height - Spacings.aboveLines -
                         Spacings.belowLines),
                0, 0, proj);
        }

        // Advance to the next frame
        gui.nextFrame();
    }
    return 0;
}
