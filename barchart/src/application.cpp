#include <stdexcept>
#include <algorithm>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.hpp"

#include "application.hpp"
#include "renderer.hpp"
#include "math.hpp"
#include "csvparser.hpp"
#include "fontrenderer.hpp"

Application::Application(Arguments arg)
  : args {arg}
{
}

struct RowState
{
  std::string name;
  long double value;
  Color color;
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
  gui.setup(800, 600, "Visualization");

  // Enable blending in GL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  math::Matrix<4, 4> proj;
  Renderer renderer;

  // Temporary placeholder spacings for now
  // Code below will modify these values based on things like font height
  // and command line arguments
  struct {
    unsigned aboveBars = 35;
    unsigned belowBars = 35;
    unsigned beforeBars = 30;
    unsigned afterBars = 30;
  } Spacings;
  // Padding values
  struct {
    const unsigned aroundRowName = 50;
    const unsigned aroundRowValue = 30;
  } Paddings;

  // Parse the provided CSV
  const std::string& fileName = args.get("-csv");
  if (fileName == Arguments::NotSet)
    throw std::runtime_error("CSV file name not provided!");
  CsvParser csv(fileName);

  // Load the provided font file
  const std::string& fontName = args.get("-font");
  if (fontName == Arguments::NotSet)
    throw std::runtime_error("Font file not provided");
  FontRenderer fontRenderer;
  fontRenderer.loadFont(fontName, 16);

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
  // Update spacings based on the longestRowName calculated
  Spacings.beforeBars = Paddings.aroundRowName
    + fontRenderer.getWidthOfMsg(longestRowName);

  // Generate colours
  generateColors(currentValues);

  // Get height of bars from arguments if set, otherwise use
  // sensible default
  unsigned barHeight = std::stoi(args.get("-barheight", 
      std::to_string(
        std::min(
          35.0,
          ((gui.height-Spacings.aboveBars-Spacings.belowBars)
           *0.9)/currentValues.size()
        )
      )
    ));

  // Draw
  while (gui.windowStillOpen())
  {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Set viewport size
    glViewport(0, 0, gui.width, gui.height);
    // Update projection matrix
    math::setOrtho(proj, 0, gui.width, gui.height, 0, -0.1f, -100.0f);

    // Sort the bars by their values
    std::sort(currentValues.begin(), currentValues.end(),
        [] (const auto& x, const auto& y) {return x.value > y.value;});
    // Adjust spacing
    Spacings.afterBars = Paddings.aroundRowValue
      + fontRenderer.getWidthOfMsg(std::to_string(currentValues.front().value));

    long double highestValue = currentValues.front().value;

    int height = Spacings.aboveBars;
    for (auto& row : currentValues)
    {
      // Get the position of the end of the bar
      int barX2 = ((gui.width - Spacings.afterBars - Spacings.beforeBars)
          *(row.value/highestValue)) + Spacings.beforeBars;

      // Draw the bar as a proportion of the largest bar
      renderer.drawBox(Spacings.beforeBars,
          height, 
          barX2,
          height + barHeight,
          row.color, proj);
      // Draw in its title
      fontRenderer.drawMsg(
          Spacings.beforeBars - (Paddings.aroundRowName/2) - fontRenderer.getWidthOfMsg(row.name),
          height, row.name, proj);

      // Draw in the current values
      fontRenderer.drawLongDouble(
          barX2 + (Paddings.aroundRowValue/2),
          height, row.value, 2, proj);

      height += barHeight + 10;
    }

    gui.nextFrame();
  }
  return 0;
}
