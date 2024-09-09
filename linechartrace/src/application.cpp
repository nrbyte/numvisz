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
    FontRenderer fontRenderer;
    FontRenderer fontRendererLarge;
    math::Matrix<4, 4> proj;

    // Parse the provided CSV
    const std::string& fileName = args.get("-csv");
    if (fileName == Arguments::NotSet)
        throw std::runtime_error("CSV file name not provided!");
    CsvParser csv(fileName);

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
    fontRenderer.loadFont(fontName, 18);
    fontRendererLarge.loadFont(fontName, 26);

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
            {row.name, Color{1.0f, 1.0f, 1.0f, 1.0f}, builder.build(), 0.0f});

        // Update longest row name
        if (row.name.size() > longestRowName.size())
            longestRowName = row.name;
    }

    // Update spacing based on longest row name
    float newSpacingAfterLines =
        fontRenderer.getWidthOfMsg(longestRowName) + Paddings.afterLines;
    if (Spacings.afterLines < newSpacingAfterLines)
        Spacings.afterLines = newSpacingAfterLines;

    // Generate colours
    generateColors(lines);

    float height = std::numeric_limits<float>().min();
    bool reachedEnd = false;
    // Start the timer and start drawing
    timer.start();
    while (gui.windowStillOpen())
    {
        gui.clearScreen(Color{1.0f, 1.0f, 1.0f, 1.0f});

        auto currentTime = timer.getInMilliseconds();
        float currentPosition = currentTime / timePerCategory;
        int intCurrentPosition = int(currentPosition);

        // Has reached end?
        if (!reachedEnd && currentPosition > csv.getCategories().size() - 1)
        {
            reachedEnd = true;
            timer.stop();
            // Stop the projection's horizontal view increasing, and ensure
            // that currentPosition and intCurrentPosition will henceforth
            // be equal to csv.getCategories().size() - 1 (i.e point to the
            // last value, and not beyond it)
            timer.setTime(timePerCategory * (csv.getCategories().size() - 1));
        }

        // Update current values
        float prevValue, nextValue, diff;
        for (auto& row : csv.getRows())
        {
            prevValue = row.values[intCurrentPosition];
            // If reached end, there is no value beyond the last
            if (reachedEnd)
                nextValue = prevValue;
            else
                nextValue = row.values[intCurrentPosition + 1];

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
        if (lines.front().currentValue > height)
            height = lines.front().currentValue;

        // Update spacing if necessary
        newSpacingAfterLines =
            Paddings.afterLines +
            fontRenderer.getWidthOfLongDouble(lines.front().currentValue,
                                              numOfDecimalPlaces);
        if (Spacings.afterLines < newSpacingAfterLines)
            Spacings.afterLines = newSpacingAfterLines;

        // Set projection
        math::setOrtho(proj, height * (1 + lineThickness / 2),
                       currentTime.count(), 0, 0, -0.1f, -100.0f);
        // Update viewport to leave space around the lines
        gui.setViewport(Spacings.beforeLines, Spacings.belowLines,
                        gui.width - Spacings.afterLines - Spacings.beforeLines,
                        gui.height - Spacings.aboveLines - Spacings.belowLines);
        // Draw the lines
        float aspectRatio = float(gui.width) / gui.height;
        for (auto& line : lines)
            line.renderer.draw(line.color, aspectRatio, lineThickness, proj);

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
        // Draw categories along the bottom
        fontRenderer.drawMsg(Spacings.beforeLines,
                             gui.height - Spacings.belowLines +
                                 Paddings.belowLines * 0.2,
                             csv.getCategories()[0], proj);
        const std::string& currentCategory =
            csv.getCategories()[intCurrentPosition];
        fontRenderer.drawMsg(gui.width - Spacings.afterLines -
                                 fontRenderer.getWidthOfMsg(currentCategory),
                             gui.height - Spacings.belowLines +
                                 Paddings.belowLines * 0.2,
                             currentCategory, proj);

        // Draw row names and values next to lines
        float nextAvailableY = 0.0f;
        for (auto& line : lines)
        {
            float textY =
                Spacings.aboveLines - fontRenderer.getFontHeight() * 0.5 +
                (1 - line.currentValue / height) *
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

            nextAvailableY = textY + fontRenderer.getFontHeight() * 1.8;
        }

        // Advance to the next frame
        gui.nextFrame();
    }
    return 0;
}
