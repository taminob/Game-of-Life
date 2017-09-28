# Game_Of_Life
This is an implemention of Conway's Game of Life using C++ and Qt. Tested with Qt 5.7 and 5.9.1 under Windows and Linux.

## How to compile
1. Download sources from https://www.github.com/sqytco/game-of-life
2. Download Qt from https://www.qt.io/download-open-source/
3. Create new directory "build" inside the source directory
4. Open command line
5. Run "qmake .." in "build"
6. Run "make" in "build"

## How to start
Start your executable (on Windows .exe) via one double click or through command line via "Game_Of_Life.exe" on Windows and through "./Game_Of_Life" on Linux.
It is possible to give the program as an argument one *.save or *.start file. These files will be loaded on start. Write on your command line after "Game_Of_Life.exe" or "./Game_Of_Life" the filename.
You can also open one of these file formats through a file manager with this program.

### *.start-file
You can pass a *.start-file as an argument to the program, open it through a file manager or open it inside the program.
The structure is easy: 0 is dead, 1 is alive. A newline marks the next row. Any other character (spaces as seperators, other numbers) will be ignored. If there are too few cells, the min. required amount will be created.

### *.save-file
You can pass a *.save-file as an argument to the program, open it through a file manager or open it inside the program.
This is the format where saved games will be stored. They are binary written and can not be read through a editor.
