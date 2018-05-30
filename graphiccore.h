// © Copyright (c) 2018 SqYtCO

#ifndef GRAPHICCORE_H
#define GRAPHICCORE_H

#include "graphicconfiguration.h"
#include "openglwidget.h"
#include "core.h"
#include <QWidget>
#include <QLabel>

// measure time of calculation while autogenerating in microseconds and output the result on the console
#define ENABLE_CALC_TIME_MEASUREMENT

// measure time of drawing cells in microseconds and output the result on the console
#define ENABLE_DRAW_TIME_MEASUREMENT

// singleton class: to get the instance call GraphicCore::get_instance()
// this core contains all graphic operations and allows some higher level operations
// it includes also some GUI elements, e.g. QFileDialog calls; this class depends on Qt
class GraphicCore
{
	// contain all graphic configurations
	GraphicConfiguration gconfig;
	// "parent" guarantees deletion of opengl/gen_counter
	QWidget parent;
	// game widget; the cells are drawn on this widget using OpenGL
	OpenGLWidget* opengl;
	// show current generation
	QLabel* gen_counter;

	// private constructors and assignment operator
	GraphicCore();
	GraphicCore(const GraphicCore&);
	GraphicCore& operator=(const GraphicCore&);

public:
	// singleton pattern - only one graphiccore per application
	static GraphicCore* get_instance();

	// update cells, calc next generation and update generation counter; calc measurement is done here
	void next_generation();
	// set all cells to given state, redraw cells and update generation counter
	void reset_cells(const Cell_State& state);
	// stop autogenerating and step-calculations and create new system with set configuration
	void new_system();

	// reset move and scale of opengl widget
	void reset_movement();
	// return true if autogenerating is running, otherwise false; it does not return true while generating one step
	bool get_generating_running();
	// start autogenerating in opengl
	void start_generating();
	// stop autogenerating in opengl
	void stop_generating();

	// update visibility, size and num of gen_counter
	void update_generation_counter();
	// redraw cells on opengl
	inline void update_opengl() { opengl->update(); }

	// read in saved game; file name will be asked in a QFileDialog
	void read_save();
	// ask for file name and write save
	void write_save_as();
	// write save to default path with date+time as name
	void write_save();

// get member
public:
	inline GraphicConfiguration* get_config() { return &gconfig; }
	inline OpenGLWidget* get_opengl_widget() { return opengl; }
	inline QLabel* get_generation_counter() { return gen_counter; }
};

#endif // GRAPHICCORE_H
