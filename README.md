# Game Of Life
This is a fast implementation of Conway's popular Game Of Life which supports many features like own rule sets and different border behaviors.


## Changes to v-2.1
- increased performance with OpenGL support and an optimized algorithm
- highly reduced memory consumption
- improved user interface
- multithreading is simplified and improved


## Compilation
### Dependencies
- libstdc++fs
- \>= Qt 5.7
- \>= C++14
### Build
cd /path/to/Game-Of-Life
mkdir build
qmake ..
make -j4
./game-of-life


## Usage
### Write own '*.gol' files:
The GOL '*.gol' file format has a simple structure:
- in the first line is the current generation saved
- each following line is a new row in the cell grid; the single cells are represented by '0' and '1'; there can be used any separator between; it is also possible to use no separator
Example:
```
11
0 0 0 0
0 1 1 0
0 1 1 0
0 0 0 0
```
This file represents 16 cells in generation 11. The 4 cells in the middle are alive, the other 12 cells are dead. The separator ' ' will be ignored.
A shorter way to write this is:
```
11
0
0110
011
0
```
Each row will be filled with dead cells. The longest row determines the width of the game board, so only one row have to have 4 cells. The separator is not necessary.


## Documentation
### Cell (cellsystem.h):

### Cell_State (basesystem.h):

### Base_System (basesystem.h):
Base_System is the virtual base class of any implemented algorithm in this application.
To use another algorithm in this application the provided interface of the class has to be implemented.
It is also possible to use algorithms which do not allow the pre-calculation of the next generation. The necessary behavior is documented in the Base_System source code.
Furthermore is it necessary to adjust the Configuration/GraphicConfiguration classes as well as the PreferencesWidget class if there are more or less features which are supported by the other algorithm.
If the same features are supported, only the member pointer in Core and the instructor calls in core.cpp have to be replaced.

### Cell_System (cellsystem.h):
This is a fast implementation with several features such as multiple border rules, own rule sets and multithreading.
It uses two std::vector which contain each X by Y instances of the "Cell"-class to manage the current and next state.
To create a new system, a new instance is required in which ctor the size has to be given as well as the behavior on the borders and the number of threads used for calculation (optional).
After the instantiation the system contains only dead cells. To fill them, it is possible to use manually set_cell() (manual calc_next_generation()-call required) and
set_all() (no additional function call required) or to use random_cells() (no additional function call required) which fill the whole system with random cells with the given ratio.
With calling next_generation() it is now possible to update the current state to the next state and calculate afterwards the new next state.
The new states can be accessed again with get_cell_state() and get_next_cell_state().

### Configuration (configuration.h):

### Core (core.h):

### GraphicConfiguration (graphicconfiguration.h):

### GraphicCore (graphiccore.h):
If ENABLE_CALC_TIME_MEASUREMENT is defined, the time for calculation of the next generation is measured and outputed on the console.
To measure the drawing performance, it is possible to define ENABLE_DRAW_TIME_MEASUREMENT to measure the time which is needed to redraw the current scene. The result is outputed on the console.

### MainWindow (mainwindow.h):

### PreferencesWidget (preferenceswidget.h):

### OpenGLWidget (openglwidget.h):
At first you have to differentiate between a step (which may also include multiple generations) and autogenerating.
Autogenerating is an infinite calculation which can be started and stopped by the 'R' key. To increase or decrease the speed of autogenerating use the "Delay" preference.
Stepping is the calculation of X generations in background a final update of the Game View, triggered by pressing 'Space'. The number of generations per step can be set in the Tool View.
Because the calculation is not slowed down by displaying, stepping can be much faster. However, to achieve that, you have to set the generations per step (in Tool View) to a high number.
The disadvantage of this mode is that there is no smooth movement of cells because of the missing updates of the screen.

### ToolWidget (toolwidget.h):

### HelpWidget (helpwidget.h):

### PreferencesWidget (preferenceswidget.h):

#### RuleButton (preferenceswidget.h):

### StartupDialog (startupdialog.h):
