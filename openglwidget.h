// © Copyright (c) 2018 SqYtCO

#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QTimer>
#include <thread>
#include <atomic>

// widget of game view
// this widget draws the cells and start calculation and generating
// signals: generating_start_stop(), new_system_created(); signals are for information purposes only
class OpenGLWidget : public QOpenGLWidget
{
	Q_OBJECT

	using big_signed = long long;

	// mouse state (pressed/released)
	bool mouse_pressed;

	// cell size in px; must be >0
	std::size_t cell_size;

	// move state in px
	big_signed move_x;
	big_signed move_y;

	// pos of 0/0-cell (to move drawn cells and determine clicked cell)
	big_signed null_pos_x;
	big_signed null_pos_y;

	big_signed drawing_pos_x;
	big_signed drawing_pos_y;

	// save previous mouse pos to calc clicked cells between two mouse move events
	QPoint previous_pos;

	std::unique_ptr<std::thread> calc_thread;

public:
	// init member and connect generating_timer
	OpenGLWidget(QWidget* parent = nullptr);
	virtual ~OpenGLWidget() override;

	// update generation counter and redraw cells
	void full_update();

	// set move_x and move_y to 0, scale to 1 and update
	void reset_movement();

protected:
	// receive events; pass all events to parent
	virtual bool eventFilter(QObject*, QEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

	// init OpenGL engine
	virtual void initializeGL() override;
	// call draw_cells and draw_grid + time measurement
	virtual void paintGL() override;
	// draw cells using OpenGL
	void draw_cells();
	// draw (if enabled) grid using OpenGL
	void draw_grid();

signals:
	// emitted when generating is started or stopped
	void generating_start_stop();
	// emitted when a new system was created
	void new_system_created();
	// emiited when cells were changed
	void cell_changed();

	// emission starts update
	void start_update();
};

#endif // OPENGLWIDGET_H
