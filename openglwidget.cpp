// © Copyright (c) 2018 SqYtCO

#include "openglwidget.h"
#include "core.h"
#include "graphiccore.h"
#include <QKeyEvent>
#ifdef ENABLE_DRAW_TIME_MEASUREMENT
#include <QDebug>
#include <chrono>
#endif

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent), scale(1), move_x(0), move_y(0), step_thread(nullptr), thread_stop(true), thread_block(false)
{
	// init OpenGL
	initializeGL();

	// connect timer with next_generation-function
	QObject::connect(&generating_timer, &QTimer::timeout, [this]() { GraphicCore::get_instance()->next_generation(); });
	qDebug("opengl");
}

OpenGLWidget::~OpenGLWidget()
{
	// join thread if existing
	if(step_thread)
		step_thread->join();
}

bool OpenGLWidget::eventFilter(QObject*, QEvent* event)
{
	// handle filtered events
	switch(event->type())
	{
		case QEvent::KeyPress:
			keyPressEvent(static_cast<QKeyEvent*>(event));
			break;
		case QEvent::MouseButtonPress:
			mousePressEvent(static_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseButtonRelease:
			mouseReleaseEvent(static_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseMove:
			mouseMoveEvent(static_cast<QMouseEvent*>(event));
			break;
		case QEvent::Wheel:
			wheelEvent(static_cast<QWheelEvent*>(event));
			break;
		default:
			QOpenGLWidget::event(event);
	}

	// pass event to parent
	return false;
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
	// mousewheel with CTRL: zoom in/out
	if(event->modifiers() == Qt::ControlModifier)
	{
		scale *= 1 + (event->angleDelta().y() / 360. * 0.3 /* = zoom-speed factor */);
	}
	// mousewheel with no modifier: move
	else if(event->modifiers() == Qt::NoModifier)
	{
		move_x += event->angleDelta().x() / 360. * 30;
		move_y += event->angleDelta().y() / 360. * 30;
	}

	update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	mouse_pressed = true;

	// calc pos of cell under mouse pointer
	std::size_t x = (event->pos().x() - null_pos_x) / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale);
	std::size_t y = (event->pos().y() - null_pos_y) / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale);

	// check boundaries
	if(x < Core::get_instance()->get_size_x() && y < Core::get_instance()->get_size_y()
		// check (event->pos() - null_pos) >= 0 to avoid rounding errors (0 <-> -cell_size)
			&& (event->pos().x() - null_pos_x) >= 0 && (event->pos().y() - null_pos_y) >= 0
		// check if cells are locked
			&& (!GraphicCore::get_instance()->get_config()->get_lock_after_first_generating() || Core::get_instance()->get_generation() == 0))
	{
		// halt background calculation
		stop_step();

		if(event->buttons() & Qt::LeftButton)
			// revive on left click if it is set, otherwise kill
			Core::get_instance()->set_cell_state(x, y,
													(GraphicCore::get_instance()->get_config()->get_left_alive_and_right_dead()) ? Cell_State::Alive : Cell_State::Dead);
		else if(event->buttons() & Qt::RightButton)
			// kill on right click if it is set, otherwise revive
			Core::get_instance()->set_cell_state(x, y,
													(GraphicCore::get_instance()->get_config()->get_left_alive_and_right_dead()) ? Cell_State::Dead : Cell_State::Alive);
	}

	update();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent*)
{
	mouse_pressed = false;

	// recalculation + update
	Core::get_instance()->calc_next_generation();
	update();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	if(mouse_pressed)
	{
		// calc pos of cell under mouse pointer
		std::size_t x = (event->pos().x() - null_pos_x) / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale);
		std::size_t y = (event->pos().y() - null_pos_y) / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale);

		// check boundaries
		if(x < Core::get_instance()->get_size_x() && y < Core::get_instance()->get_size_y()
			// check (event->pos() - null_pos) >= 0 to avoid rounding errors (0 <-> -cell_size)
				&& (event->pos().x() - null_pos_x) >= 0 && (event->pos().y() - null_pos_y) >= 0
			// check if cells are locked
				&& (!GraphicCore::get_instance()->get_config()->get_lock_after_first_generating() || Core::get_instance()->get_generation() == 0))
		{
			stop_step();

			if(event->buttons() & Qt::LeftButton)
				// revive on left click if it is set, otherwise kill
				Core::get_instance()->set_cell_state(x, y, (GraphicCore::get_instance()->get_config()->get_left_alive_and_right_dead()) ? Cell_State::Alive : Cell_State::Dead);
			else if(event->buttons() & Qt::RightButton)
				// kill on right click if it is set, otherwise revive
				Core::get_instance()->set_cell_state(x, y, (GraphicCore::get_instance()->get_config()->get_left_alive_and_right_dead()) ? Cell_State::Dead : Cell_State::Alive);

			update();
		}
	}
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
	// move: [up/down/left/right] arrow
	// fast move: CTRL + [up/down/left/right] arrow
	if(event->key() == Qt::Key_Up)
	{
		move_y += 10;
		if(event->modifiers() == Qt::ControlModifier)
			move_y += 100;
	}
	else if(event->key() == Qt::Key_Down)
	{
		move_y -= 10;
		if(event->modifiers() == Qt::ControlModifier)
			move_y -= 100;
	}
	else if(event->key() == Qt::Key_Left)
	{
		move_x += 10;
		if(event->modifiers() == Qt::ControlModifier)
			move_x += 100;
	}
	else if(event->key() == Qt::Key_Right)
	{
		move_x -= 10;
		if(event->modifiers() == Qt::ControlModifier)
			move_x -= 100;
	}
	// zoom: CTRL + [+/-]
	else if((event->modifiers() & Qt::CTRL) && event->key() == Qt::Key_Plus)
		scale *= 1.1;
	else if((event->modifiers() & Qt::CTRL) && event->key() == Qt::Key_Minus)
		scale *= 0.9;
	// next generation: space
	else if(event->key() == Qt::Key_Space)
	{
		if(thread_stop)
			generate_step();
		else
			stop_step();
		// avoid double repainting
		return;
	}
	// start/stop autogenerating: r
	else if(event->key() == Qt::Key_R)
	{
		// if autogenerating is running: stop
		if(generating_timer.isActive())
			stop_generating();
		// if autogenerating is stopped: start
		else
		{
			stop_step();

			start_generating();
			// avoid double repainting
			return;
		}
	}
	// set all cells dead: a
	// set all cells alive: CTRL + a
	else if(event->key() == Qt::Key_A)
	{
		stop_generating();

		if(event->modifiers() == Qt::NoModifier)
			GraphicCore::get_instance()->reset_cells(Cell_State::Dead);
		else if(event->modifiers() == Qt::ControlModifier)
			GraphicCore::get_instance()->reset_cells(Cell_State::Alive);
	}
	// new random cells: n
	else if(event->key() == Qt::Key_N)
	{
		GraphicCore::get_instance()->new_system();
		emit new_system_generated();
	}
	// reset position: m
	else if(event->key() == Qt::Key_M)
	{
		reset_movement();
	}

	update();
}

void OpenGLWidget::initializeGL()
{
	// setup OpenGL viewport and mode
	glViewport(0, 0, width(), height());
	glMatrixMode(GL_PROJECTION);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void OpenGLWidget::paintGL()
{
#ifdef ENABLE_DRAW_TIME_MEASUREMENT
	// begin of time measuring
	auto begin = std::chrono::high_resolution_clock::now();
#endif

	// load matrix
	glLoadIdentity();

	// set 0/0 to top-left-corner
	glOrtho(0, width(), height(), 0, -1, 1);

	// set null_pos central (moved by move_x/move_y (also scaled)) and scaled by scale-factor
	null_pos_x = move_x * scale + (width() / 2) - (Core::get_instance()->get_size_x() * GraphicCore::get_instance()->get_config()->get_cell_size() / 2) * scale;
	null_pos_y = move_y * scale + (height() / 2) - (Core::get_instance()->get_size_y() * GraphicCore::get_instance()->get_config()->get_cell_size() / 2) * scale;

	// move view to correct pos
	glTranslated(null_pos_x, null_pos_y, 0);

	// increse or decrease size by scale-factor (set in wheelEvent); has to be done after glTranslated in order to get right position
	glScaled(scale, scale, scale);

	// set background (+ dead cell) color
	glClearColor(GraphicCore::get_instance()->get_config()->get_background_color().red() / 255., GraphicCore::get_instance()->get_config()->get_background_color().green() / 255., GraphicCore::get_instance()->get_config()->get_background_color().blue() / 255., 1);

	draw_cells();

	draw_grid();

#ifdef ENABLE_DRAW_TIME_MEASUREMENT
	// give out measured time
	auto end = std::chrono::high_resolution_clock::now();
	qDebug() << "drawing: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs";
#endif
}

void OpenGLWidget::draw_cells()
{
	// calc visible cells
	std::size_t x_begin = -(null_pos_x) / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale);
	std::size_t y_begin = -(null_pos_y) / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale);
	std::size_t x_end = x_begin + width() / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale) + 3;
	std::size_t y_end = y_begin + height() / (GraphicCore::get_instance()->get_config()->get_cell_size() * scale) + 3;

	// check boundaries
	if(x_begin > Core::get_instance()->get_size_x())
		x_begin = 0;
	if(y_begin > Core::get_instance()->get_size_y())
		y_begin = 0;
	if(x_end <= x_begin || x_end > Core::get_instance()->get_size_x())
		x_end = Core::get_instance()->get_size_x();
	if(y_end <= y_begin || y_end > Core::get_instance()->get_size_y())
		y_end = Core::get_instance()->get_size_y();

	// draw dead cells as background (performance improvement)
	glColor3ub(GraphicCore::get_instance()->get_config()->get_dead_color().red(),
			   GraphicCore::get_instance()->get_config()->get_dead_color().green(),
			   GraphicCore::get_instance()->get_config()->get_dead_color().blue());
	glRecti(x_begin * GraphicCore::get_instance()->get_config()->get_cell_size(), y_begin * GraphicCore::get_instance()->get_config()->get_cell_size(),
			x_end * GraphicCore::get_instance()->get_config()->get_cell_size(), y_end * GraphicCore::get_instance()->get_config()->get_cell_size());

	std::size_t x1 = 0, y1 = 0, x2, y2;
	// draw line by line visible cells
	for(std::size_t a = y_begin; a < y_end; ++a)
	{
		// y-coord of right lower corner
		y2 = (a + 1) * GraphicCore::get_instance()->get_config()->get_cell_size();

		for(std::size_t b = x_begin; b < x_end; ++b)
		{
			x2 = (b + 1) * GraphicCore::get_instance()->get_config()->get_cell_size();

			// alive cells
			if(Core::get_instance()->get_cell_state(b, a))
			{
				if(Core::get_instance()->get_next_cell_state(b, a))
					// alive cell color
					glColor3ub(GraphicCore::get_instance()->get_config()->get_alive_color().red(), GraphicCore::get_instance()->get_config()->get_alive_color().green(), GraphicCore::get_instance()->get_config()->get_alive_color().blue());
				else
					// dying cell color
					glColor3ub(GraphicCore::get_instance()->get_config()->get_dying_color().red(), GraphicCore::get_instance()->get_config()->get_dying_color().green(), GraphicCore::get_instance()->get_config()->get_dying_color().blue());

				glRecti(x1, y1, x2, y2);
			}
			// dead but in next generation reviving cells
			else if(Core::get_instance()->get_next_cell_state(b, a))
			{
				// reviving cell color
				glColor3ub(GraphicCore::get_instance()->get_config()->get_reviving_color().red(), GraphicCore::get_instance()->get_config()->get_reviving_color().green(), GraphicCore::get_instance()->get_config()->get_reviving_color().blue());
				glRecti(x1, y1, x2, y2);
			}

			x1 = x2;
		}

		//
		y1 = y2;
		// start each row
		x1 = 0;
	}
}

void OpenGLWidget::draw_grid()
{
	if(GraphicCore::get_instance()->get_config()->get_grid_active())
	{
		glColor3f(0.5, 0.5, 0.5);	// fixed line color
		glLineWidth(0.5);			// fixed line width
		std::size_t max_line_coord_x = Core::get_instance()->get_size_x() * GraphicCore::get_instance()->get_config()->get_cell_size();
		std::size_t max_line_coord_y = Core::get_instance()->get_size_y() * GraphicCore::get_instance()->get_config()->get_cell_size();
		for(std::size_t line_coord = 0; line_coord <= max_line_coord_x || line_coord <= max_line_coord_y; line_coord += GraphicCore::get_instance()->get_config()->get_cell_size())
		{
			if(line_coord <= max_line_coord_x)
			{
				glBegin(GL_LINES);
				glVertex2i(line_coord, 0);
				glVertex2i(line_coord, max_line_coord_y);
				glEnd();
			}
			if(line_coord <= max_line_coord_y)
			{
				glBegin(GL_LINES);
				glVertex2i(0, line_coord);
				glVertex2i(max_line_coord_x, line_coord);
				glEnd();
			}
		}
	}
}

void OpenGLWidget::reset_movement()
{
	move_x = 0;
	move_y = 0;
	scale = 1;

	update();
}

void OpenGLWidget::generate_step()
{
	if(thread_stop && !thread_block)
	{
		thread_stop = false;
		step_thread = new std::thread(&OpenGLWidget::calc_generations,
															   this, GraphicCore::get_instance()->get_config()->get_generations_per_step());
	}
}

void OpenGLWidget::stop_step()
{
	thread_stop = true;
	if(step_thread)
	{
		step_thread->join();
		delete step_thread;
		step_thread = nullptr;
	}
}

bool OpenGLWidget::get_generating_running()
{
	return generating_timer.isActive();
}

void OpenGLWidget::start_generating()
{
	stop_step();
	thread_block = true;
	// start repeated calc after set delay
	generating_timer.setSingleShot(false);
	generating_timer.setInterval(GraphicCore::get_instance()->get_config()->get_delay());
	generating_timer.start();

	emit generating_start_stop();
}

void OpenGLWidget::stop_generating()
{
	generating_timer.stop();
	thread_block = false;

	emit generating_start_stop();
}

void OpenGLWidget::calc_generations(std::size_t generations)
{
	while(generations--)
	{
		if(!thread_stop)
			Core::get_instance()->next_generation();
	}

	thread_stop = true;
	GraphicCore::get_instance()->update_generation_counter();
	update();
}
