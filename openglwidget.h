// © Copyright (c) 2018 SqYtCO

#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QTimer>
#include <thread>

// widget of game view
// this widget draws the cells and start calculation and generating
// signals: generating_start_stop(), new_system_generated(); signals are information purposes only
class OpenGLWidget : public QOpenGLWidget
{
	Q_OBJECT

	// mouse state (pressed/released)
	bool mouse_pressed;

	// zoom state (1=no zoom; 1.1=zoom in; 0.9=zoom out)
	double scale;

	// move state in px
	int move_x;
	int move_y;

	// pos of 0/0-cell (to move drawn cells and determine clicked cell)
	int null_pos_x;
	int null_pos_y;

	// stepping
	std::thread* step_thread;			// thread to calculate step
	std::atomic_bool thread_stop;		// variable to interrupt thread
	bool thread_block;					// block thread starting (while autogenerating)

	// calculation function which is connected to step_thread
	void calc_generations(std::size_t generations);

	// autogenerating
	QTimer generating_timer;			// timer to sync redraw and calculation

public:
	// init OpenGL and connect generating_timer
	OpenGLWidget(QWidget* parent = nullptr);
	// join step_thread
	virtual ~OpenGLWidget();

	// set move_x and move_y to 0, scale to 1 and update
	void reset_movement();

	// generate generations set in "generations_per_step"
	void generate_step();
	// stop step-generating
	void stop_step();
	// true if autogenerating is running otherwise false
	bool get_generating_running();
	// start autogenerating
	void start_generating();
	// stop autogenerating
	void stop_generating();

protected:
	// receive events; pass all events to parent
	virtual bool eventFilter(QObject*, QEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

	// setup OpenGL and call draw_cells and draw_grid + time measurement
	void paintGL();
	// draw cells using OpenGL
	inline void draw_cells();
	// draw (if enabled) grid using OpenGL
	inline void draw_grid();

signals:
	// emitted when generating is started or stopped
	void generating_start_stop();
	// emitted when a new system was created
	void new_system_generated();
};

#endif // OPENGLWIDGET_H
