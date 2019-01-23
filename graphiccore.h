// © Copyright (c) 2018 SqYtCO

#ifndef GRAPHICCORE_H
#define GRAPHICCORE_H

#include "core.h"
#include "graphicconfiguration.h"
#include <memory>
#include <thread>
#include <mutex>

class OpenGLWidget;
class QLabel;

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
	static GraphicConfiguration gconfig;

	// game widget; the cells are drawn on this widget using OpenGL
	static OpenGLWidget* opengl;
	// show current generation
	static QLabel* gen_counter;

	// stepping
	static std::unique_ptr<std::thread> stepping_thread;	// thread to calculate step
	static std::atomic_bool stepping_stop;					// variable to interrupt step calculations
	static bool stepping_block;								// block stepping (while autogenerating)

	// autogenerating
	static std::unique_ptr<std::thread> generating_thread;	// thread for autogenerating
	static std::atomic_bool generating_stop;				// variable to interrupt autogenerating

	// calculating
	static std::unique_ptr<std::thread> calc_thread;		// thread for background calculation
	static std::atomic_bool calc_stop;						// variable to interrupt calculation

	static std::mutex system_mutex;

	static void next_generations(std::size_t generations);

public:
	// has to be called after creating QApplication
	static void init();
	// has to be called
	static void init_gui(OpenGLWidget* opengl, QLabel* gen_counter);

	// set all cells to given state, redraw cells and update generation counter
	static void reset_cells(Cell_State state);
	// stop autogenerating and step-calculations and create new system with set configuration
	static void new_system();

	// reset move and scale of opengl widget
	static void reset_movement();

	// return true if autogenerating is running, otherwise false; it does not return true while generating a step (use step_running())
	static inline bool generating_running() { return !generating_stop; }
	// start autogenerating
	static void start_generating();
	// stop autogenerating
	static void stop_generating();

	// start background calculation of next generation
	static void calc_next_generation();
	// wait for end of calculation
	static void wait_for_calculation();

	// generate step
	static void step();
	// interrupt calculation of step
	static void stop_step();
	// return true if calculation of step is running
	static bool step_running() { return !stepping_stop; }
	// block any new attempt to start calculation of step if true
	static void block_step(bool blocked) { stepping_block = blocked; }

	// redraw cells on opengl
	static void update_opengl();
	// update visibility, size and num of gen_counter
	static void update_generation_counter();

	// read in saved game; file name will be asked in a QFileDialog
	static void read_save();
	// ask for file name and write save
	static void write_save_as();
	// write save to default path with date+time as name
	static void write_save();

// get member
public:
	static inline GraphicConfiguration* get_config() { return &gconfig; }
	static inline std::mutex& get_mutex() { return system_mutex; }
};

#endif // GRAPHICCORE_H
