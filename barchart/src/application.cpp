#include <stdexcept>
#include <algorithm>
#include <cmath>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.hpp"

#include "application.hpp"
#include "renderer.hpp"
#include "math.hpp"
#include "csvparser.hpp"
#include "fontrenderer.hpp"
#include "timer.hpp"

Application::Application(Arguments arg)
  : args {arg}
{
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

// Helper function to generate colors for each row
static void generateColors(std::vector<RowState>& rows)
{
  int i = 0;
  float increment = 3*(1.0f/rows.size());
  for (float r = 0.0f; r <= 1.0f; r += increment)
  {
    for (float g = 0.0f; g <= 1.0f; g += increment)
    {
      for (float b = 0.0f; b <= 1.0f; b += increment)
      {
        if (i < rows.size())
          rows[i].color = Color {r, g, b, 1.0f};
        else return;
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
  Renderer renderer;
  FontRenderer fontRenderer;
  FontRenderer fontRendererLarge;
  // Enable blending in GL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Temporary placeholder spacings for now
  // Code below will modify these values based on things like font height
  // and command line arguments
  struct {
    unsigned aboveBars = 35;
    unsigned belowBars = 35;
    unsigned beforeBars = 30;
    unsigned afterBars = 30;

    unsigned beforeControl = 0;
    unsigned afterControl = 0;
  } Spacings;
  // Padding values
  struct {
    const unsigned aroundRowName = 50;
    const unsigned aroundRowValue = 30;
    const unsigned aroundTitle = 80;
    const unsigned aroundControl = 80; // Around the time control at the bottom
  } Paddings;

  // Parse the provided CSV
  const std::string& fileName = args.get("-csv");
  if (fileName == Arguments::NotSet)
    throw std::runtime_error("CSV file name not provided!");
  CsvParser csv(fileName);

  // Go through each row, and put in the starting value, and also
  // get the longest row name, for measurements later in the code
  std::vector<RowState> currentValues;
  std::string longestRowName = "";
  for (auto& row : csv.getRows())
  {
    currentValues.push_back({row.name, row.values.front(),
        Color {1.0f, 1.0f, 1.0f, 1.0f}});
    if (row.name.length() > longestRowName.length())
      longestRowName = row.name;
  }

  // Get height of bars from arguments if set, otherwise use
  // sensible default
  unsigned barHeight = args.getInt("-barheight",
      std::min(40.0, ((gui.height - Spacings.aboveBars - Spacings.belowBars)
          *0.9)/currentValues.size()));

  // Load the provided font file
  const std::string& fontName = args.get("-font");
  if (fontName == Arguments::NotSet)
    throw std::runtime_error("Font file not provided");
  fontRenderer.loadFont(fontName, barHeight * 0.36);
  fontRendererLarge.loadFont(fontName, barHeight*0.6);

  // Get time per category from arguments if set, other use
  // sensible default
  Timer::FloatMS timePerBar {args.getInt("-timepercategory", 2000)};

  // Update spacings
  Spacings.beforeBars = Paddings.aroundRowName
    + fontRenderer.getWidthOfMsg(longestRowName);
  Spacings.aboveBars = Paddings.aroundTitle +
    fontRendererLarge.getFontHeight();
  Spacings.belowBars = Paddings.aroundControl +
    fontRenderer.getFontHeight();

  Spacings.beforeControl = Paddings.aroundControl +
    fontRenderer.getWidthOfMsg(csv.getCategories().front());
  Spacings.afterControl = Paddings.aroundControl +
    fontRenderer.getWidthOfMsg(csv.getCategories().back());

  // Generate colours
  generateColors(currentValues);

  // Start the timer and start drawing
  timer.start();
  while (gui.windowStillOpen())
  {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Set viewport size
    glViewport(0, 0, gui.width, gui.height);
    // Update projection matrix
    math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);
    
    auto currentTime = timer.getInMilliseconds();
    float currentPosition = currentTime/timePerBar;

    // Don't go past the last category
    bool reachedEnd = false;
    if (currentPosition > csv.getCategories().size()-1)
    {
      reachedEnd = true;
      currentPosition = csv.getCategories().size()-1;
    }

    // 1 - Draw title and category
    fontRendererLarge.drawMsg(Paddings.aroundTitle, Paddings.aroundTitle/2,
        csv.getName(), proj);
    fontRendererLarge.drawMsg(gui.width - Paddings.aroundTitle
        - fontRendererLarge.getWidthOfMsg(csv.getCategories()[int(currentPosition)]),
        Paddings.aroundTitle/2, csv.getCategories()[int(currentPosition)],
        proj);

    // 2 - Calculate the current row state (the values for each row)
    for (auto& row : csv.getRows())
    {
      long double currentValue = 0.0;
      if (reachedEnd)
        // If we've reached the end, each row should display its last value
        currentValue = row.values[csv.getCategories().size()-1];
      else {
        // Get the values based on the 2 categories we're inbetween
        long double baseValue = row.values[int(currentPosition)];
        long double nextValue = row.values[int(currentPosition+1)];
        long double diff = nextValue - baseValue;
        // The current value is the current category's value plus
        // a percentage amount of the difference between this cateogory and
        // the next. Therefore, it gives the view that we're animating towards
        // the next value.
        currentValue = baseValue +
          (diff*(currentPosition-int(currentPosition)));
      }

      // Update the row's current value
      std::find_if(currentValues.begin(), currentValues.end(),
          [&] (const auto& x) { return x.name == row.name; })
        ->value = currentValue;
    }

    // 3 - Sort the bars by their values
    std::sort(currentValues.begin(), currentValues.end(),
        [] (const auto& x, const auto& y) {return x.value > y.value;});
    // 4 - Adjust spacing
    Spacings.afterBars = Paddings.aroundRowValue
      + fontRenderer.getWidthOfMsg(std::to_string(currentValues.front().value));

    // 5 - Update the heights of the rows
    for (int i = 0; i < currentValues.size(); i++)
    {
      RowState& rs = currentValues[i];
      rs.heightAim = Spacings.aboveBars + (i*(barHeight + 10));
      if (rs.currentHeight == 0)
        rs.currentHeight = rs.heightAim;
    }

    // 6 - draw the rows and their surrounding text
    long double highestValue = currentValues.front().value;
    int height = Spacings.aboveBars;
    // Used below to make the font draw in the middle of the bar
    long fontHeightSpacing = (barHeight - fontRenderer.getFontHeight())/2;
    for (auto& row : currentValues)
    {
      if (row.currentHeight + barHeight > (gui.height - Spacings.belowBars))
        continue;

      // Get the position of the end of the bar
      int barX2 = ((gui.width - Spacings.afterBars - Spacings.beforeBars)
          *(row.value/highestValue)) + Spacings.beforeBars;

      // Draw the bar as a proportion of the largest bar
      renderer.drawBox(Spacings.beforeBars,
          row.currentHeight, 
          barX2,
          row.currentHeight + barHeight,
          row.color, proj);
      // Draw in its title
      fontRenderer.drawMsg(
          Spacings.beforeBars - (Paddings.aroundRowName*0.3)
          - fontRenderer.getWidthOfMsg(row.name),
          row.currentHeight+fontHeightSpacing, row.name, proj);
      // Draw in the current values
      fontRenderer.drawLongDouble(
          barX2 + (Paddings.aroundRowValue*0.5),
          row.currentHeight+fontHeightSpacing, row.value, 2, proj);

      // Update the heights
      int diff = row.heightAim - row.currentHeight;
      // Make sure numbers like 0.4 and -0.2 are rounded to 1 and
      // -1 respectively, as by default they'd be truncated to 0 - which will
      // cause the difference in height to remain and not shrink
      row.currentHeight +=
        (diff < 0) ? std::floor(diff/5.0) : std::ceil(diff/5.0);
    }

    // 7 - Draw time control
    float controlX2 = gui.width - Spacings.afterControl;
    float controlWidth = gui.width - Spacings.beforeControl
                       - Spacings.afterControl;
    renderer.drawBox(Spacings.beforeControl,
        gui.height - Spacings.belowBars*0.8,
        controlX2, gui.height - Spacings.belowBars*0.75,
        Color { 0,0,0,1 }, proj);
    fontRenderer.drawMsg(Spacings.beforeControl
        + (controlWidth * std::min(
            1.0f, (currentTime/(csv.getCategories().size()*timePerBar)))),
        gui.height - Spacings.belowBars*0.72,
        csv.getCategories()[currentPosition], proj);

    // Advance to the next frame
    gui.nextFrame();
  }
  return 0;
}
