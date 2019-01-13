// © Copyright (c) 2018 SqYtCO

#include "openglwidget.h"
#include "core.h"
#include "graphiccore.h"
#include <QKeyEvent>
#include <mutex>
#include <cmath>
#ifdef ENABLE_DRAW_TIME_MEASUREMENT
#include <QDebug>
#include <chrono>
#endif

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent), cell_size(GraphicCore::get_config()->get_cell_size()), move_x(0), move_y(0)
{
	// if update has to be done from other thread
	QObject::connect(this, &OpenGLWidget::start_update, this, &OpenGLWidget::full_update);
}

OpenGLWidget::~OpenGLWidget()
{
	GraphicCore::wait_for_calculation();
	GraphicCore::stop_step();
	GraphicCore::stop_generating();
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
		double factor = (1 + (event->angleDelta().y() / 360. * 0.3 /* = zoom-speed factor */));
		factor = (factor <= 0) ? 0.1 : factor;
		if(static_cast<std::size_t>(cell_size * factor) <= 1)
			cell_size = 1;
		else if(static_cast<std::size_t>(cell_size * factor) == cell_size)
		{
			if(factor > 0)
				++cell_size;
			else
				--cell_size;
		}
		else if(static_cast<std::size_t>(cell_size * factor) > 8 * GraphicCore::get_config()->get_cell_size())
			cell_size = 8 * GraphicCore::get_config()->get_cell_size();
		else
			cell_size *= factor;
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
	std::size_t x = static_cast<std::size_t>((event->pos().x() - null_pos_x) / cell_size);
	std::size_t y = static_cast<std::size_t>((event->pos().y() - null_pos_y) / cell_size);

	// check boundaries
	if(x < Core::get_size_x() && y < Core::get_size_y()
		// check (event->pos() - null_pos) >= 0 to avoid rounding errors (0 <-> -cell_size)
			&& (event->pos().x() - null_pos_x) >= 0 && (event->pos().y() - null_pos_y) >= 0
		// check if cells are locked
			&& (!GraphicCore::get_config()->get_lock_after_first_generating() || Core::get_generation() == 0)
		// check if cells are greater than 1px
			&& cell_size > 1)
	{
		// halt background calculation
		GraphicCore::stop_step();

		std::lock_guard<decltype(GraphicCore::get_mutex())> system_lock(GraphicCore::get_mutex());
		if(event->buttons() & Qt::LeftButton)
			// revive on left click if it is set, otherwise kill
			Core::set_cell_state(x, y,
													(GraphicCore::get_config()->get_left_button_alive_right_dead()) ? Alive : Dead);
		else if(event->buttons() & Qt::RightButton)
			// kill on right click if it is set, otherwise revive
			Core::set_cell_state(x, y,
													(GraphicCore::get_config()->get_left_button_alive_right_dead()) ? Dead : Alive);
	}

	previous_pos = event->pos();
	emit cell_changed();
	update();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent*)
{
	mouse_pressed = false;

	GraphicCore::calc_next_generation();

	emit cell_changed();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	if(mouse_pressed)
	{
		int dif_x = previous_pos.x() - event->pos().x();
		int dif_y = previous_pos.y() - event->pos().y();

		bool dif_x_active = std::abs(dif_x) >= cell_size;
		bool dif_y_active = std::abs(dif_y) >= cell_size;

		while(dif_x_active || dif_y_active)
		{
			dif_x_active = std::abs(dif_x) >= cell_size;
			dif_y_active = std::abs(dif_y) >= cell_size;

			if(dif_x_active)
			{
				if(dif_x > 0)
					dif_x -= cell_size;
				else
					dif_x += cell_size;
			}
			if(dif_y_active)
			{
				if(dif_y > 0)
					dif_y -= cell_size;
				else
					dif_y += cell_size;
			}

			QPoint temp(event->pos().x() + dif_x, event->pos().y() + dif_y);

			// calc pos of cell under mouse pointer
			std::size_t x = static_cast<std::size_t>((temp.x() - null_pos_x) / cell_size);
			std::size_t y = static_cast<std::size_t>((temp.y() - null_pos_y) / cell_size);

			if(x < Core::get_size_x() && y < Core::get_size_y()
					&& (temp.x() - null_pos_x) >= 0 && (temp.y() - null_pos_y) >= 0
					&& (!GraphicCore::get_config()->get_lock_after_first_generating() || Core::get_generation() == 0))
			{
				GraphicCore::stop_step();

				std::lock_guard<decltype(GraphicCore::get_mutex())> system_lock(GraphicCore::get_mutex());
				if(event->buttons() & Qt::LeftButton)
					// revive on left click if it is set, otherwise kill
					Core::set_cell_state(x, y, (GraphicCore::get_config()->get_left_button_alive_right_dead()) ? Alive : Dead);
				else if(event->buttons() & Qt::RightButton)
					// kill on right click if it is set, otherwise revive
					Core::set_cell_state(x, y, (GraphicCore::get_config()->get_left_button_alive_right_dead()) ? Dead : Alive);
			}
		}

		// calc pos of cell under mouse pointer
		std::size_t x = static_cast<std::size_t>((event->pos().x() - null_pos_x) / cell_size);
		std::size_t y = static_cast<std::size_t>((event->pos().y() - null_pos_y) / cell_size);

		// check boundaries
		if(x < Core::get_size_x() && y < Core::get_size_y()
			// check (event->pos() - null_pos) >= 0 to avoid rounding errors (0 <-> -cell_size)
				&& (event->pos().x() - null_pos_x) >= 0 && (event->pos().y() - null_pos_y) >= 0
			// check if cells are locked
				&& (!GraphicCore::get_config()->get_lock_after_first_generating() || Core::get_generation() == 0))
		{
			GraphicCore::stop_step();

			std::lock_guard<decltype(GraphicCore::get_mutex())> system_lock(GraphicCore::get_mutex());
			if(event->buttons() & Qt::LeftButton)
				// revive on left click if it is set, otherwise kill
				Core::set_cell_state(x, y, (GraphicCore::get_config()->get_left_button_alive_right_dead()) ? Alive : Dead);
			else if(event->buttons() & Qt::RightButton)
				// kill on right click if it is set, otherwise revive
				Core::set_cell_state(x, y, (GraphicCore::get_config()->get_left_button_alive_right_dead()) ? Dead : Alive);

			previous_pos = event->pos();
			emit cell_changed();
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
		move_y += GraphicCore::get_config()->get_cell_size();
		if(event->modifiers() == Qt::ControlModifier)
			move_y += GraphicCore::get_config()->get_cell_size() * 10;
	}
	else if(event->key() == Qt::Key_Down)
	{
		move_y -= GraphicCore::get_config()->get_cell_size();
		if(event->modifiers() == Qt::ControlModifier)
			move_y -= GraphicCore::get_config()->get_cell_size() * 10;
	}
	else if(event->key() == Qt::Key_Left)
	{
		move_x += GraphicCore::get_config()->get_cell_size();
		if(event->modifiers() == Qt::ControlModifier)
			move_x += GraphicCore::get_config()->get_cell_size() * 10;
	}
	else if(event->key() == Qt::Key_Right)
	{
		move_x -= GraphicCore::get_config()->get_cell_size();
		if(event->modifiers() == Qt::ControlModifier)
			move_x -= GraphicCore::get_config()->get_cell_size() * 10;
	}
	// zoom: [+/-]
	else if(event->key() == Qt::Key_Plus)
	{
		if(cell_size <= 10)
			++cell_size;
		else if(cell_size < 8 * GraphicCore::get_config()->get_cell_size())
			cell_size *= 1.25;
	}
	else if(event->key() == Qt::Key_Minus)
	{
		if(cell_size <= 10 && cell_size > 1)
			--cell_size;
		else if(cell_size > 1)
			cell_size *= 0.8;
	}
	// next generation: space
	else if(event->key() == Qt::Key_Space)
	{
		if(GraphicCore::step_running())
			GraphicCore::stop_step();
		else
			GraphicCore::step();
		// avoid double repainting
		return;
	}
	// start/stop autogenerating: r
	else if(event->key() == Qt::Key_R)
	{
		// if autogenerating is running: stop
		if(GraphicCore::generating_running())
			GraphicCore::stop_generating();
		// if autogenerating is stopped: start
		else
		{
			GraphicCore::stop_step();
			GraphicCore::start_generating();
			// avoid double repainting
			return;
		}
	}
	// set all cells dead: a
	// set all cells alive: CTRL + a
	else if(event->key() == Qt::Key_A)
	{
		GraphicCore::stop_generating();

		if(event->modifiers() == Qt::NoModifier)
			GraphicCore::reset_cells(Dead);
		else if(event->modifiers() == Qt::ControlModifier)
			GraphicCore::reset_cells(Alive);
	}
	// new random cells: n
	else if(event->key() == Qt::Key_N)
	{
		GraphicCore::new_system();
		emit new_system_created();
	}
	// reset position: m
	else if(event->key() == Qt::Key_M)
	{
		reset_movement();
	}
	else
		return;

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
	std::lock_guard<decltype(GraphicCore::get_mutex())> system_lock(GraphicCore::get_mutex());

#ifdef ENABLE_DRAW_TIME_MEASUREMENT
	// begin of time measuring
	auto begin = std::chrono::high_resolution_clock::now();
#endif

	// load matrix
	glLoadIdentity();

	// set 0/0 to top-left-corner
	glOrtho(0, width(), height(), 0, -1, 1);

	// set null_pos central (moved by move_x/move_y and scaled by scale-factor)
	null_pos_x = move_x + (width() / 2) - (Core::get_size_x() / 2) * cell_size;
	null_pos_y = move_y + (height() / 2) - (Core::get_size_y() / 2) * cell_size;

	// expand if borders are visible
	while(Core::get_config()->get_border_behavior() == Border_Behavior::Borderless &&
			// check right border
			((null_pos_x + (static_cast<long long>(Core::get_size_x() * cell_size) - width())) <= 0 ||
			// check lower border
			 (null_pos_y + (static_cast<long long>(Core::get_size_y() * cell_size) - height())) <= 0 ||
			// check left and upper borders
			 (null_pos_x) > 0 || (null_pos_y) > 0))
	{
		if(!Core::expand())
				break;
		null_pos_x = move_x + (width() / 2) - (Core::get_size_x() / 2) * cell_size;
		null_pos_y = move_y + (height() / 2) - (Core::get_size_y() / 2) * cell_size;
		emit new_system_created();
	}

	// set background (+ dead cell) color
	glClearColor(GraphicCore::get_config()->get_background_color().red() / 255.,
				 GraphicCore::get_config()->get_background_color().green() / 255.,
				 GraphicCore::get_config()->get_background_color().blue() / 255., 1);

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
	std::size_t x_begin = -(null_pos_x) / cell_size;
	std::size_t y_begin = -(null_pos_y) / cell_size;
	std::size_t x_end = x_begin + width() / cell_size + 3;
	std::size_t y_end = y_begin + height() / cell_size + 3;

	// check boundaries
	if(x_begin > Core::get_size_x())
		x_begin = 0;
	if(y_begin > Core::get_size_y())
		y_begin = 0;
	if(x_end <= x_begin || x_end > Core::get_size_x())
		x_end = Core::get_size_x();
	if(y_end <= y_begin || y_end > Core::get_size_y())
		y_end = Core::get_size_y();

	// draw dead cells as background (performance improvement)
	glColor3ub(GraphicCore::get_config()->get_dead_color().red(),
			   GraphicCore::get_config()->get_dead_color().green(),
			   GraphicCore::get_config()->get_dead_color().blue());

	std::size_t graphic_size_x = Core::get_size_x() * cell_size;
	std::size_t graphic_size_y = Core::get_size_y() * cell_size;

	// if field is out of sight, don't draw any cell
	if(null_pos_x > width() + static_cast<long>(graphic_size_x) || null_pos_x < (-1 * static_cast<long>(graphic_size_x)) ||
		null_pos_y > height() + static_cast<long>(graphic_size_y) || null_pos_y < (-1 * static_cast<long>(graphic_size_y)))
		return;

	long real_null_pos_x = (null_pos_x < 0) ? (null_pos_x % static_cast<long>(cell_size)) : (null_pos_x > width()) ? width() : null_pos_x;
	long real_null_pos_y = (null_pos_y < 0) ? (null_pos_y % static_cast<long>(cell_size)) : (null_pos_y > height()) ? height() : null_pos_y;

	if(static_cast<long>(graphic_size_x) + null_pos_x < width())
	{
		if(static_cast<long>(graphic_size_y) + null_pos_y < height())
			glRecti(real_null_pos_x, real_null_pos_y, null_pos_x + graphic_size_x, null_pos_y + graphic_size_y);
		else
			glRecti(real_null_pos_x, real_null_pos_y, null_pos_x + graphic_size_x, height());
	}
	else
	{
		if(static_cast<long>(graphic_size_y) + null_pos_y < height())
			glRecti(real_null_pos_x, real_null_pos_y, width(), null_pos_y + graphic_size_y);
		else
			glRecti(real_null_pos_x, real_null_pos_y, width(), height());
	}

	long x1 = real_null_pos_x, y1 = real_null_pos_y, x2, y2;
	// draw line by line visible cells
	for(std::size_t a = y_begin; a < y_end; ++a)
	{
		// y-coord of right lower corner
		y2 = (a - y_begin + 1) * cell_size + real_null_pos_y;

		for(std::size_t b = x_begin; b < x_end; ++b)
		{
			x2 = (b - x_begin + 1) * cell_size + real_null_pos_x;

			// alive cells
			if(Core::get_cell_state(b, a))
			{
				// if autogenerating is running, use only alive color
				if(GraphicCore::generating_running() || Core::get_next_cell_state(b, a))
					// alive cell color
					glColor3ub(GraphicCore::get_config()->get_alive_color().red(),
							   GraphicCore::get_config()->get_alive_color().green(), GraphicCore::get_config()->get_alive_color().blue());
				else
					// dying cell color
					glColor3ub(GraphicCore::get_config()->get_dying_color().red(),
							   GraphicCore::get_config()->get_dying_color().green(), GraphicCore::get_config()->get_dying_color().blue());

				glRecti(x1, y1, x2, y2);
			}
			// dead but in next generation reviving cells; use only if autogenerating is not running
			else if(!GraphicCore::generating_running() && Core::get_next_cell_state(b, a))
			{
				// reviving cell color
				glColor3ub(GraphicCore::get_config()->get_reviving_color().red(),
						   GraphicCore::get_config()->get_reviving_color().green(), GraphicCore::get_config()->get_reviving_color().blue());
				glRecti(x1, y1, x2, y2);
			}

			x1 = x2;
		}

		y1 = y2;
		// start each row
		x1 = real_null_pos_x;
	}
}

void OpenGLWidget::draw_grid()
{
	if(GraphicCore::get_config()->get_grid_active())
	{
		// actual null position
		long real_null_pos_x = (null_pos_x < 0) ? (null_pos_x % static_cast<long>(cell_size)) : (null_pos_x > width()) ? width() : null_pos_x;
		long real_null_pos_y = (null_pos_y < 0) ? (null_pos_y % static_cast<long>(cell_size)) : (null_pos_y > height()) ? height() : null_pos_y;
		long x_end = width() + 3 * cell_size + real_null_pos_x;
		long y_end = height() + 3 * cell_size + real_null_pos_y;

		glColor3f(0.5, 0.5, 0.5);			// fixed line color
		glLineWidth(1.0);					// fixed line width

		for(long line_coord_x = real_null_pos_x, line_coord_y = real_null_pos_y; line_coord_x <= x_end || line_coord_y <= y_end; line_coord_x += cell_size, line_coord_y += cell_size)
		{
			if(line_coord_x <= x_end)
			{
				glBegin(GL_LINES);
				glVertex2i(line_coord_x, real_null_pos_y);
				glVertex2i(line_coord_x, y_end);
				glEnd();
			}
			if(line_coord_y <= y_end)
			{
				glBegin(GL_LINES);
				glVertex2i(real_null_pos_x, line_coord_y);
				glVertex2i(x_end, line_coord_y);
				glEnd();
			}
		}
	}
}

void OpenGLWidget::full_update()
{
	GraphicCore::update_generation_counter();
	update();
}

void OpenGLWidget::reset_movement()
{
	move_x = 0;
	move_y = 0;
	cell_size = GraphicCore::get_config()->get_cell_size();

	update();
}
