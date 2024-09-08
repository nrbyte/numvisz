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
    Timer::FloatMS timePerCategory{args.getInt("-timepercategory", 200)};

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
            {row.name, Color{1.0f, 1.0f, 1.0f, 1.0f}, builder.build()});
    }

    // Generate colours
    generateColors(lines);

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

        // Set projection
        math::setOrtho(proj, 25000, currentTime.count(), 0, 0, -0.1f, -100.0f);
        // Draw the lines
        for (auto& line : lines)
            line.renderer.draw(line.color, proj);

        // Advance to the next frame
        gui.nextFrame();
    }
    return 0;
}
