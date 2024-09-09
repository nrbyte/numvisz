#include <stdexcept>
#include <cmath>

#include "application.hpp"

#include "viszbase/linerenderer.hpp"
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
    Timer timer;
    math::Matrix<4, 4> proj;
    // Setup a window, MUST NOT CALL ANY OPENGL BEFORE THIS
    gui.setup(800, 600, "Visualization");

    // Parse the provided CSV
    const std::string& fileName = args.get("-csv");
    if (fileName == Arguments::NotSet)
        throw std::runtime_error("CSV file name not provided!");
    CsvParser csv(fileName);

    // Get time per category from arguments if set, otherwise use
    // sensible default
    Timer::FloatMS timePerCategory{args.getInt("-timepercategory", 100)};

    // Get line thickness from arguments if set, otherwise use
    // sensible default
    float lineThickness = args.getInt("-linethickness", 10);
    lineThickness /= 1000;

    // Go through each line, and load in the values
    std::vector<Line> lines;
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
    }

    // Generate colours
    generateColors(lines);

    float height = 0.0f;
    bool reachedEnd = false;
    // Start the timer and start drawing
    timer.start();
    while (gui.windowStillOpen())
    {
        gui.clearScreen(Color{1.0f, 1.0f, 1.0f, 1.0f});
        // Set viewport size
        gui.setViewport(0, 0, gui.width, gui.height);
        // Update projection matrix
        math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);

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
        float highestValue = std::numeric_limits<float>().min();
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

            // Update the highest value
            if (currentValue > highestValue)
                highestValue = currentValue;
        }

        // Resize the height to ensure the highest value can be seen
        if (highestValue > height)
            height = highestValue;

        // Set projection
        math::setOrtho(proj, height * (1 + lineThickness / 2),
                       currentTime.count(), 0, 0, -0.1f, -100.0f);
        // Draw the lines
        float aspectRatio = float(gui.width) / gui.height;
        for (auto& line : lines)
            line.renderer.draw(line.color, aspectRatio, lineThickness, proj);

        // Advance to the next frame
        gui.nextFrame();
    }
    return 0;
}