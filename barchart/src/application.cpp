#include <stdexcept>
#include <algorithm>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.hpp"

#include "application.hpp"
#include "renderer.hpp"
#include "math.hpp"
#include "csvparser.hpp"

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

  // Parse the provided CSV
  const std::string& fileName = args.get("-csv", Arguments::NotSet);
  if (fileName == Arguments::NotSet)
    throw std::runtime_error("CSV file name not provided!");
  CsvParser csv(fileName);

  // Go through each row, and put in the starting value
  std::vector<RowState> currentValues;
  for (auto& row : csv.getRows())
    currentValues.push_back({row.name, row.values.front(), Color {1.0f, 1.0f, 1.0f, 1.0f}});

  // Generate colours
  generateColors(currentValues);

  // Temporary placeholder spacings for now
  struct {
    unsigned aboveBars = 35;
    unsigned belowBars = 35;
    unsigned beforeBars = 50;
    unsigned afterBars = 50;
  } Spacings;

  // Get height of bars from arguments if set, otherwise use
  // sensible default
  unsigned barHeight = std::stoi(args.get("-barheight", 
      std::to_string(
        std::min(
          50.0,
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

    long double highestValue = currentValues.front().value;

    int height = Spacings.aboveBars;
    for (auto& row : currentValues)
    {
      // Draw the bar as a proportion of the largest bar
      renderer.drawBox(Spacings.beforeBars,
          height, 
          ((gui.width - Spacings.afterBars - Spacings.beforeBars)
           *(row.value/highestValue)) + Spacings.beforeBars,
          height + barHeight,
          row.color, proj);

      height += barHeight + 10;
    }


    gui.nextFrame();
  }
  return 0;
}
