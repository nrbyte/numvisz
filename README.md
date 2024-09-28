<h1 align="center">
 Numvisz 📊
</h1>

## About
Numvisz is a program that lets you make animated visualizations of data. The program is written in C++ and utilizes Qt for the GUI, and OpenGL for the visualization.

Below are some photos showing what the program looks like. When you use the program yourself the bars/lines will move. For the bar chart race, you can change the time by clicking and dragging your mouse along the timeline on the bottom.

<p align="center">
<img src="https://github.com/user-attachments/assets/5f023c0c-e721-49b5-ad2d-6ec57fe6e639" />
<img src="https://github.com/user-attachments/assets/dc416532-fcb5-4efc-bf25-e30b8b973dda" />
 NOTE: Data is GDP per capita, PPP (constant 2021 international $) for a select few countries shown in the picture. Sourced from the World Bank [here](https://data.worldbank.org/indicator/NY.GDP.PCAP.PP.KD?locations=US-DE-FR-JP-GB-CA-IT-EU)
<img src="https://github.com/user-attachments/assets/7fc4b138-56db-4849-b1ad-95149f5c5a04" />
 NOTE: Data is random numbers - not from a real dataset.
</p>



## Structure
The project has 4 CMake targets, the Numvisz picker GUI executable (allows visualizations to be picked), the visualization base shared library, the barchartrace executable, and the linechartrace executable. The picker GUI tool launches the other executables with the necessary command line arguments. The root CMakeLists configures the exeuctables to be placed next to eachother in a bin directory within the build directory, so that the GUI can launch the others.

## Code style
The project's code style is specified using clang-format to be based on the LLVM style, plus the following modifications: braces on seperate lines, * and & to appear next to the type rather than the name, indentation to be of width 4, and access modifiers to have no space infront of them.

## Compiling
The dependencies of the program include Qt (Core, Widgets, Sql), GLFW and Freetype, along with their respective dependencies.

Note the instructions below assume you have an installation of the Qt SDK on your system, containing the libraries and headers required for compilation.
You could alternatively add Qt as a dependency under the vcpkg manifest - make sure you specify which features you require though, otherwise you can end up compiling everything (which will take a long time!)
### Windows
For Windows, its recommended you compile with vcpkg for GLFW and Freetype, and direct CMake to an installation of Qt.
Note you most likely will need the Windows SDK.

Below is a set of sample commands you can run to generate a build directory and compile:

```
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=<your path to Qt e.g: C:\Qt\6.7.2\msvc2019_64> -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake ..
cmake --build .
```
Alternatively, you can open the project in an IDE like QT creator which already has vcpkg integration. Visual Studio should also work provided you tell it where to find Qt either through the extension or manually.

To run the picker executable (the GUI) you will need <your path to Qt\bin e.g: C:\Qt\6.7.2\msvc2019_64\bin> in your path or it will complain that it can't find Qt DLLs. Qt Creator automatically tells the exe where the DLLs are. You can also run windeployqt which will place the DLLs next to the exe, although this tool is more for deployment.

### Linux
Most distributions should have the listed dependencies in the system's repositories.
Use your system's package management tool to install the dependencies listed above and then the commands below should work.

```
mkdir build
cd build
cmake ..
cmake --build .
```

You can alternatively use vcpkg.

