#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "application.hpp"

#include "viszbase/renderer.hpp"
#include "viszbase/math.hpp"
#include "viszbase/csvparser.hpp"
#include "viszbase/fontrenderer.hpp"
#include "viszbase/timer.hpp"

#include "barchart.hpp"

Application::Application(Arguments arg) : args{arg} {}

struct RowState
{
    std::string name;
    long double value;
    Color color;
    // To animate the bar moving positions
    int currentHeight;
    int heightAim;
};


int Application::run()
{
    Timer timer;
    math::Matrix<4, 4> proj;
    // Setup a window, MUST NOT CALL ANY OPENGL BEFORE THIS
    gui.setup(800, 600, "Visualization");
    // Initialize utility classes
    Renderer renderer;
    FontRenderer fontRenderer;
    FontRenderer fontRendererLarge;
    // Store number of decimal places for drawing numbers
    int numOfDecimalPlaces = args.getInt("-decimalplaces", 0);

    // Temporary placeholder spacings for now
    // Code below will modify these values based on things like font height
    // and command line arguments
    struct
    {
        unsigned aboveBars = 35;
        unsigned belowBars = 35;
        unsigned beforeBars = 30;
        unsigned afterBars = 30;

        unsigned beforeControl = 0;
        unsigned afterControl = 0;
    } Spacings;
    // Padding values
    struct
    {
        const unsigned aroundRowName = 50;
        const unsigned aroundRowValue = 50;
        const unsigned aroundTitle = 80;
        const unsigned aroundControl =
            80; // Around the time control at the bottom
    } Paddings;

    // Get the CSV path
    const std::string& fileName = args.get("-csv");
    if (fileName == Arguments::NotSet)
        throw std::runtime_error("CSV file name not provided!");

    // Get height of bars from arguments if set, otherwise use a default of 35
    unsigned barHeight = args.getInt("-barheight", 35);

    // Get time per category from arguments if set, other use
    // sensible default
    Timer::FloatMS timePerCategory{args.getInt("-timepercategory", 2000)};

    // Use the arguments above to create the barchart class
    BarChart barChart(fileName, timePerCategory, barHeight);

    // Load the provided font file
    const std::string& fontName = args.get("-font");
    if (fontName == Arguments::NotSet)
        throw std::runtime_error("Font file not provided");
    fontRenderer.loadFont(fontName, barHeight * 0.36);
    fontRendererLarge.loadFont(fontName, barHeight * 0.6);

    // Update spacings
    Spacings.beforeBars =
        Paddings.aroundRowName +
        fontRenderer.getWidthOfMsg(barChart.getLongestRowName());
    Spacings.aboveBars =
        Paddings.aroundTitle + fontRendererLarge.getFontHeight();
    Spacings.belowBars = Paddings.aroundControl + fontRenderer.getFontHeight();

    Spacings.beforeControl =
        Paddings.aroundControl +
        fontRenderer.getWidthOfMsg(barChart.getCategories().front());
    Spacings.afterControl =
        Paddings.aroundControl +
        fontRenderer.getWidthOfMsg(barChart.getCategories().back());

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

        // 1 - Update bar chart values based on the current time, and the height
        // of each bar based on the space to leave above the bars respectively
        barChart.update(currentTime, Spacings.aboveBars);

        // 2 - Draw title and category
        fontRendererLarge.drawMsg(Paddings.aroundTitle,
                                  Paddings.aroundTitle / 2.0,
                                  barChart.getName(), proj);
        fontRendererLarge.drawMsg(
            gui.width - Paddings.aroundTitle -
                fontRendererLarge.getWidthOfMsg(barChart.getCurrentCategory()),
            Paddings.aroundTitle / 2.0, barChart.getCurrentCategory(), proj);

        // 3 - Adjust spacing
        float newAfterBarsValue =
            Paddings.aroundRowValue +
            fontRenderer.getWidthOfLongDouble(barChart.getHighestValue(),
                                              numOfDecimalPlaces);
        // Only increase the spacing if more space is required
        if (newAfterBarsValue > Spacings.afterBars)
            Spacings.afterBars = newAfterBarsValue;

        // 4 - draw background lines to better indicate how the bars are moving
        // Calculate the values the lines will be at by using log10, therefore,
        // a value like 35,000 will have lines every 10,000 (through the integer
        // conversion).
        long double highestValue = barChart.getHighestValue();
        long double lineSeperation =
            std::pow(10, (int)std::log10(highestValue));
        int amountOfLines = highestValue / lineSeperation;
        // If there are more than 5 lines, or less than 3 lines, double or half
        // the distance between the lines respectively
        if (amountOfLines > 5)
        {
            lineSeperation *= 2;
            amountOfLines = highestValue / lineSeperation;
        }
        if (amountOfLines < 3)
        {
            lineSeperation /= 2;
            amountOfLines = highestValue / lineSeperation;
        }
        // Draw the lines using the same proportion calculation used to draw
        // bars
        for (int i = 1; i <= amountOfLines; i++)
        {
            long double lineValue = lineSeperation * i;
            float lineX =
                ((gui.width - Spacings.afterBars - Spacings.beforeBars) *
                 (lineValue / highestValue)) +
                Spacings.beforeBars;

            renderer.drawBox(lineX, Spacings.aboveBars - 10, lineX + 2,
                             gui.height - Spacings.belowBars,
                             Color{0, 0, 0, 0.3}, proj);
        }

        // 5 - draw the rows and their surrounding text
        // Used below to make the font draw in the middle of the bar
        long fontHeightSpacing = (barHeight - fontRenderer.getFontHeight()) / 2;
        for (auto& row : barChart.getRowStates())
        {
            if (row.currentHeight + barHeight <
                (gui.height - Spacings.belowBars))
            {
                // Get the position of the end of the bar
                float barX2 =
                    ((gui.width - Spacings.afterBars - Spacings.beforeBars) *
                     (row.value / highestValue)) +
                    Spacings.beforeBars;

                // Draw the bar as a proportion of the largest bar
                renderer.drawBox(Spacings.beforeBars, row.currentHeight, barX2,
                                 row.currentHeight + barHeight, row.color,
                                 proj);
                // Draw in its title
                fontRenderer.drawMsg(
                    Spacings.beforeBars - (Paddings.aroundRowName * 0.3) -
                        fontRenderer.getWidthOfMsg(row.name),
                    row.currentHeight + fontHeightSpacing, row.name, proj);
                // Draw in the current values
                fontRenderer.drawLongDouble(
                    barX2 + (Paddings.aroundRowValue * 0.3),
                    row.currentHeight + fontHeightSpacing, row.value,
                    numOfDecimalPlaces, proj);
            }
        }

        // 6 - Draw time control
        float controlX2 = gui.width - Spacings.afterControl;
        float controlWidth =
            gui.width - Spacings.beforeControl - Spacings.afterControl;
        renderer.drawBox(Spacings.beforeControl,
                         gui.height - Spacings.belowBars * 0.8, controlX2,
                         gui.height - Spacings.belowBars * 0.75,
                         Color{0, 0, 0, 1}, proj);
        // Draw the current category underneath the time control
        float currentCategoryPercent = barChart.getCurrentPosition() /
                                       (barChart.getCategories().size() - 1);
        fontRenderer.drawMsg(Spacings.beforeControl +
                                 (controlWidth * currentCategoryPercent),
                             gui.height - Spacings.belowBars * 0.72,
                             barChart.getCurrentCategory(), proj);

        // 7 - Handle mouse input, draw the category the mouse is over if it is
        // in range, and handle when the mouse is clicked
        float percentOfControl =
            (gui.mouseX - Spacings.beforeControl) / controlWidth;
        if (gui.mouseY > (gui.height - Spacings.belowBars * 0.90) &&
            gui.mouseY < (gui.height - Spacings.belowBars * 0.5) &&
            gui.mouseX > Spacings.beforeControl &&
            gui.mouseX < (gui.width - Spacings.afterControl))
        {
            // Get the category hovered over by working out how far along the
            // mouse is on the time control as a percentage (percentOfControl,
            // calculated above this if statement), and multiplying it by the
            // total amount of categories
            const std::string& hoverCategory = barChart.getCategories().at(
                (barChart.getCategories().size() - 1) * percentOfControl);
            // Draw the category just above the time control
            fontRenderer.drawMsg(gui.mouseX,
                                 gui.height - Spacings.belowBars * 0.8 -
                                     fontRenderer.getFontHeight(),
                                 hoverCategory, proj);

            // If the mouse is down, set the time using the percentage
            // calculated above
            if (gui.leftMouseDown)
            {
                timer.stop();
            }
        }
        // If the timer is stopped, which happens above when the user clicks
        // on the timeline control, then set the time based on the mouse's X
        // position
        if (timer.isStopped())
        {
            timer.setTime(Timer::FloatMS{
                (timePerCategory * (barChart.getCategories().size() - 1)) *
                std::min(1.0f, std::max(0.0f, percentOfControl))});
        }
        // If the user is not holding down their mouse, the timer should not
        // be stopped
        if (!gui.leftMouseDown)
            timer.resume();

        // Advance to the next frame
        gui.nextFrame();
    }
    return 0;
}
