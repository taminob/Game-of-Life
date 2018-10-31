# Game Of Life
This is a fast implementation of Conway's popular Game Of Life which supports many features like own rule sets and different border behaviors.


## Changes to v-3.0
- HashLife implementation added to simulate infinite fields
- cell drawing improvements


## Compilation
### Dependencies
- libstdc++fs (GCC)
- opengl32 (MSVC)
- \>= Qt 5.9.1
- \>= C++17

### Build
#### Requirements
- writeable directory
- qmake installed and added to PATH
- make (GCC), nmake (MSVC)

#### Build process
cd /path/to/Game-Of-Life
mkdir build
qmake ..
make -j4 (GCC) or nmake (MSVC)


## Usage
### Program start
The program can be started in a command line:
```
Linux/MacOS:
./game-of-life
Windows:
game-of-life.exe
```
or depending on the operating system in the GUI. The start of programs in the GUI of the OS is usually implemented with a double click.

### Getting started
For questions, please use the help, which can be opened by the shortcut 'H'.

### Write own '*.gol' files:
The GOL '*.gol' file format has a simple structure:
- in the first line is the current generation saved
- each following line is a new row in the cell grid; the single cells are represented by '0' and '1'; there can be used any separator between, but no separator is required
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
### Cell_State (basesystem.h):
This enumeration defines the two cell states Alive and Dead.

### Cell (cellsystem.h):
Each instance represents one cell which holds the Cell_State in its only property "state".

### Base_System (basesystem.h):
Base_System is the virtual base class of any implemented algorithm in this application.
To use another algorithm in this application the pure virtual functions of the class has to be implemented.
It is also possible to use algorithms which do not allow the pre-calculation of the next generation. The necessary behavior is documented in the Base_System source code.
Furthermore it is necessary to adjust the Configuration/GraphicConfiguration classes as well as the PreferencesWidget class if there are more or less features which are offered by the other algorithm.
If the same features are supported, only the member pointer in Core and the instructor calls in core.cpp have to be replaced.

### Cell_System (cellsystem.h; inherits from Base_System):
This is a fast implementation with several features such as multiple border rules, own rule sets and multithreading.
It uses two std::vector which contain each X by Y instances of the "Cell"-class to represent the current and next state of all cells.
To update to the next generation only the used system is swapped and the other system contains now the next state. This is calculated by calc_next_generation() which is implicitly called in next_generation().
Every new system requires a new instance in which ctor the size, the rule sets, the behavior on the borders and the number of threads used for calculation (optional) can be specified.
The rule sets are stored as bitmask and have to be given as bit mask. They are built up that the bit 0 represents that 0 neighbors are necessary to survive/be born while the bit 1 represents that 1 neighbor is necessary to survive/be born.
The bits can be combined freely and without limitation. It is possible to use up to including 8 neighbors to build rule sets.
After the instantiation the system contains only dead cells. To fill them, it is possible to use manually set_cell() (manual calc_next_generation()-call required afterwards) and
set_all() (no additional function call required) or to use random_cells() (no additional function call required) which fills the whole system with random cells with the given ratio between living and dead cells.
Now it is now possible to update the current state to the next state and calculate afterwards the new next state by calling next_generation().
The new states can be accessed again with get_cell_state() and get_next_cell_state().

### Configuration (configuration.h):
This class stores all non-graphic configurations.
It is also responsible for loading and saving its configurations. The file name is ".configuration" and the path can be set with set_config_path(). The default path is the execution path.

### Core (core.h):
The singleton class Core allows a higher level usage of Cell_System and is independent of the graphical interface.
It is also possible to use this Core and build a GUI-less application by providing another kind of user interface.
Furthermore you can replace the used Qt-GUI by any other framework which allows the include of C++.
The class provides the possibility to manage preferences using the Configuration class (get_config()), to get and manipulate cells, to get the current generation which is upcount with each call of next_generation()
and to save/load cell patterns.

### GraphicConfiguration (graphicconfiguration.h):
This class stores all graphic configurations which are required to allow a customized graphical interface.
It is also responsible for loading and saving its configurations. The file name is ".configuration" and the path can be set with set_config_path(). The default path is the execution path.
GraphicConfiguration requires the QColor class of the Qt-framework.

### GraphicCore (graphiccore.h):
This singleton class provides a defined higher level interface to OpenGLWidget. It has to be initialized by GraphicCore::init() and the graphic members OpenGLWidget* and QLabel* has to set by using GraphicCore::init_gui(). GraphicCore does not take over the ownership.
It contains also the graphical configurations in the form of an instance of GraphicConfiguration which can be get by a get_config()-call.
Using the defines ENABLE_CALC_TIME_MEASUREMENT and ENABLE_DRAW_TIME_MEASUREMENT it is possible to measure the calculation and drawing performance and output the result on the execution console in microseconds.
GraphicCore requires the Qt-framework.

### MainWindow (mainwindow.h; inherits from QMainWindow):


### OpenGLWidget (openglwidget.h; inherits from QOpenGLWidget):
This widget class contains the main part of the game view, the game board. It is responsible for the fast and correct OpenGL drawing and manages autogenerating and stepping. The game shortcuts are handled here.
To achieve high performance only the visible part of the cells is drawn and the dead cells are drawn as one great background rect. The living/dying/reviving cells are drawn on top. This avoids many OpenGL-function calls.
When generating you have to differentiate between a step (which may also include multiple generations) and autogenerating.
Autogenerating is an infinite calculation which can be started and stopped by the 'R' key. To increase or decrease the speed of autogenerating use the "Delay" preference.
Stepping is the calculation of X generations in background a final update of the Game View, triggered by pressing 'Space'. The number of generations per step can be set in the Tool View.
Because the calculation is not slowed down by displaying, stepping can be much faster. However, to achieve that, you have to set the generations per step (in Tool View) to a high number.
The disadvantage of this mode is that there is no smooth movement of cells because of the missing updates of the screen.

### ToolWidget (toolwidget.h; inherits from QFrame):
The ToolWidget class is the widget of the Tool View which provides some buttons for all kind of actions and some configuration options like generations_per_step and the possibility to change the mouse actions.

### HelpWidget (helpwidget.h; inherits from QFrame):
This class is the widget of the Help View. It contains a big HTML viewer where the help text is displayed, a button which jumps to the index-anchor of the html-document and a button which allows to hide the Help View.

### PreferencesWidget (preferenceswidget.h; inherits from QFrame):
The PreferencesWidget class is the place where all configurations are in the GUI accessible.
It contains GUI elements for displaying and changing preferences and put them together in one view.

#### RuleButton (preferenceswidget.h; inherits from QToolButton):
This small class is a customized button which is used to allow configuring rules.
It contains a display number and a state which is toggled on a click.

### StartupDialog (startupdialog.h; inherits from QDialog):
This dialog is shown after startup if it is not diabled with GraphicConfiguration::show_startup_dialog = false. By default it is shown after startup.
The configuration property can be changed by unselecting the checkbox in this dialog.
The dialog contains a introduction and explains some basic shortcuts.
To redirect the user to the help view a button is integreted.
