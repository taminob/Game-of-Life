// © Copyright (c) 2018 SqYtCO

#include "mainwindow.h"
#include "graphiccore.h"
#include "startupdialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>

MainWindow::MainWindow(const char* start_file, QWidget* parent) : QMainWindow(parent),
											preferences_view(this), preferences_animation(&preferences_view, "pos"),
											tool_view(this), tool_animation(&tool_view, "pos"),
											help_view(this), help_animation(&help_view, "pos")
{
	if(start_file != nullptr)
	{
		Core::get_instance()->load(start_file);
		tool_view.update_current_size_label();
	}

	// setup GUI and set current active view
	init_GUI();
	current_view = Game_View;

	// set minimum size
	this->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

	// set strongest focus policy
	this->setFocusPolicy(Qt::WheelFocus);

	// enable events in opengl-widget
	this->installEventFilter(GraphicCore::get_instance()->get_opengl_widget());

	// setup resize_timer; this timer fixes a resize bug when switching to fullscreen mode
	resize_timer.setInterval(10);
	resize_timer.setSingleShot(true);
	QObject::connect(&resize_timer, &QTimer::timeout, this, &MainWindow::update_views_geometry);

	// connect signals of views with slots
	QObject::connect(&preferences_view, &PreferencesWidget::hide_preferences, this, &MainWindow::hide_preference_view);
	QObject::connect(&preferences_view, &PreferencesWidget::language_changed, this, &MainWindow::translate_application);
	QObject::connect(&preferences_view, &PreferencesWidget::color_changed, &tool_view, &ToolWidget::update_mouse_previews);
	QObject::connect(&tool_view, &ToolWidget::hide_tool, this, &MainWindow::hide_tool_view);
	QObject::connect(&tool_view, &ToolWidget::show_help, this, &MainWindow::show_help_view);
	QObject::connect(&tool_view, &ToolWidget::show_preferences, this, &MainWindow::show_preference_view);
	QObject::connect(&tool_view, &ToolWidget::fullscreen_changed, [this]()
	{
		if(isFullScreen())
			showMaximized();
		else
			showFullScreen();
	});
	QObject::connect(&help_view, &HelpWidget::hide_help, this, &MainWindow::hide_help_view);
	QObject::connect(GraphicCore::get_instance()->get_opengl_widget(), &OpenGLWidget::generating_start_stop, &tool_view, &ToolWidget::update_play_stop_button);
	QObject::connect(GraphicCore::get_instance()->get_opengl_widget(), &OpenGLWidget::new_system_generated, &tool_view, &ToolWidget::update_current_size_label);

	translate_application();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	// show/hide preferences view
	if(event->key() == Qt::Key_P)
	{
		if(current_view & Preferences_View)
			hide_preference_view();
		else
		{
			GraphicCore::get_instance()->stop_generating();
			show_preference_view();
		}
	}
	// show/hide help view
	else if(event->key() == Qt::Key_H)
	{
		if(current_view & Help_View)
			hide_help_view();
		else
		{
			GraphicCore::get_instance()->stop_generating();
			show_help_view();
		}
	}
	// show tool view only if neither preferences nor help view are active
	else if(event->key() == Qt::Key_T && !(current_view & (Preferences_View | Help_View)))
	{
		if(current_view & Tool_View)
			hide_tool_view();
		else
			show_tool_view();
	}
	// open only if neither preferences nor help view are active
	else if(event->key() == Qt::Key_O && !(current_view & (Preferences_View | Help_View)))
	{
		GraphicCore::get_instance()->read_save();
		tool_view.update_current_size_label();
	}
	// save only if neither preferences nor help view are active
	else if(event->key() == Qt::Key_S && !(current_view & (Preferences_View | Help_View)))
	{
		if(event->modifiers() & Qt::ControlModifier)
			GraphicCore::get_instance()->write_save_as();
		else
			GraphicCore::get_instance()->write_save();
	}
	else if(event->key() == Qt::Key_F)
	{
		if(isFullScreen())
			showMaximized();
		else
			showFullScreen();
	}
	else if(event->key() == Qt::Key_Q)
		close();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
	resize_timer.start();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	GraphicCore::get_instance()->stop_generating();

	// if any preference is not saved, show question; this question is necessary if the user is in the preferences view and tries to close the window
	if(!(Core::get_instance()->get_config()->get_saved() && GraphicCore::get_instance()->get_config()->get_saved()))
	{
		QMessageBox::StandardButton preferences_save_answer = QMessageBox::question(this, tr("Save Preferences?"), tr("Save changed preferences?"),
																					QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if(preferences_save_answer == QMessageBox::Discard)
			preferences_view.discard_changes();
		else if(preferences_save_answer == QMessageBox::Save)
			preferences_view.apply_changes();
		else
		{
			event->ignore();
			return;
		}
	}

	if(GraphicCore::get_instance()->get_config()->get_exit_warning())
	{
		QMessageBox::StandardButton quit_answer = QMessageBox::question(this, tr("Save Game?"), tr("Save current game?"),
																		QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if(quit_answer == QMessageBox::Discard)
		{
			event->accept();
		}
		else if(quit_answer == QMessageBox::Save)
		{
			GraphicCore::get_instance()->write_save();
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
}

void MainWindow::show()
{
	// read fullscreen preference and start in set mode
	if(GraphicCore::get_instance()->get_config()->get_fullscreen())
		this->showFullScreen();
	else
		this->showMaximized();

	if(GraphicCore::get_instance()->get_config()->get_show_startup_dialog())
	{
		StartupDialog dialog;
		QObject::connect(&dialog, &StartupDialog::show_help, this, &MainWindow::show_help_view);
		dialog.exec();
	}

	resize_timer.start();
}

void MainWindow::init_GUI()
{
	// remove white borders
	game_layout.setMargin(0);

	// init game view (main view)
	game_layout.addWidget(GraphicCore::get_instance()->get_opengl_widget(), 0, 0, 2, 2);
	game_layout.addWidget(GraphicCore::get_instance()->get_generation_counter(), 1, 1);
	game_view.setLayout(&game_layout);

	// hide preferences view
	preferences_view.setVisible(false);

	// hide help view
	help_view.setVisible(false);

	// hide tool view
	tool_view.setVisible(false);

	// set central widget
	setCentralWidget(&game_view);

	// set z axis order
	tool_view.raise();
	help_view.raise();
	preferences_view.raise();
}

void MainWindow::translate_application()
{
	if(GraphicCore::get_instance()->get_config()->get_language() == Language::German ||
			(GraphicCore::get_instance()->get_config()->get_language() == Language::System && QLocale::system().language() == QLocale::German))
	{
		if(!translator.load(":/translations/gol_de"))
			qDebug("No DE translation found!");
		if(!qt_translator.load(":/translations/qtbase_de"))
			qDebug("No DE_Qt translation found!");

		qApp->installTranslator(&translator);
		qApp->installTranslator(&qt_translator);
	}
	else
	{
		// turn off translation; switch to native English
		qApp->removeTranslator(&translator);
		qApp->removeTranslator(&qt_translator);

	}

	preferences_view.translate();
	tool_view.translate();
	help_view.translate();
}

void MainWindow::update_views_geometry()
{
	// update size of views
	tool_view.resize(this->width(), ToolWidget::TOOL_HEIGHT);
	preferences_view.resize(this->size());
	help_view.resize(this->width() / 3, this->height());

	// update pos of views
	if(current_view & Preferences_View)
	{
		// show
		show_preference_view();
		if(!(current_view & Help_View))
			help_view.move(-help_view.width(), game_view.y());
		tool_view.move(game_view.y(), -tool_view.height());
	}
	else if(current_view & Help_View)
	{
		show_help_view();
		if(!(current_view & Preferences_View))
			preferences_view.move(width(), game_view.y());
		tool_view.move(game_view.y(), -tool_view.height());
	}
	else if(current_view & Tool_View)
	{
		show_tool_view();
		preferences_view.move(width(), game_view.y());
		help_view.move(-help_view.width(), game_view.y());
	}
	else
	{
		preferences_view.move(width(), game_view.y());
		help_view.move(-help_view.width(), game_view.y());
		tool_view.move(game_view.y(), -tool_view.height());
	}
}

void MainWindow::show_game_view()
{
	hide_preference_view();
	hide_help_view();
}

void MainWindow::show_preference_view()
{
	// make it visible
	preferences_view.show();

	// hide tool view but keep bit set
	if(current_view & Tool_View)
	{
		tool_animation.setStartValue(tool_view.pos());
		tool_animation.setEndValue(QPoint(game_view.x(), -tool_view.height()));
		tool_animation.setDuration(ANIMATION_TIME);
		tool_animation.stop();
		tool_animation.start();

		// disable focus in tool view
		tool_view.disable_focus();
		this->setFocus();
		// disable events in tool view
		this->removeEventFilter(&tool_view);
	}

	if(current_view & Help_View)
	{
		preferences_animation.setStartValue(preferences_view.pos());
		preferences_animation.setEndValue(QPoint(help_view.width(), game_view.y()));
		preferences_animation.setDuration(ANIMATION_TIME);
		preferences_animation.stop();
		preferences_animation.start();
	}
	else
	{
		preferences_animation.setStartValue(preferences_view.pos());
		preferences_animation.setEndValue(QPoint(width() - preferences_view.width(), game_view.y()));
		preferences_animation.setDuration(ANIMATION_TIME);
		preferences_animation.stop();
		preferences_animation.start();
	}

	// enable focus in preferences view
	preferences_view.enable_focus();
	// enable events in preferences view; disable events in game view (opengl)
	this->removeEventFilter(GraphicCore::get_instance()->get_opengl_widget());
	this->installEventFilter(&preferences_view);
	current_view |= Preferences_View;
}

void MainWindow::hide_preference_view()
{
	// if any preference is not saved, show question
	if(!(Core::get_instance()->get_config()->get_saved() && GraphicCore::get_instance()->get_config()->get_saved()))
	{
		QMessageBox::StandardButton preferences_save_answer = QMessageBox::question(this, tr("Save Preferences?"), tr("Save changed preferences?"),
																					QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if(preferences_save_answer == QMessageBox::Discard)
			preferences_view.discard_changes();
		else if(preferences_save_answer == QMessageBox::Save)
			preferences_view.apply_changes();
		else
			return;
	}

	// restore tool view if it is active and help view is not active
	if((current_view & Tool_View) && !(current_view & Help_View))
		show_tool_view();

	preferences_animation.setStartValue(preferences_view.pos());
	preferences_animation.setEndValue(QPoint(width(), game_view.y()));
	preferences_animation.setDuration(ANIMATION_TIME);
	preferences_animation.stop();
	preferences_animation.start();

	// disable focus in preferences view
	preferences_view.disable_focus();
	this->setFocus();
	// disable events in preferences view; enable events in game view (opengl)
	this->removeEventFilter(&preferences_view);
	this->installEventFilter(GraphicCore::get_instance()->get_opengl_widget());
	// clear preferences view bit
	current_view &= ~Preferences_View;
}

void MainWindow::show_tool_view()
{
	tool_view.show();
	tool_animation.setStartValue(tool_view.pos());
	tool_animation.setEndValue(QPoint(game_view.x(), game_view.y()));
	tool_animation.setDuration(ANIMATION_TIME);
	tool_animation.stop();
	tool_animation.start();

	// enable focus in tool view
	tool_view.enable_focus();
	// enable events in tool view
	this->installEventFilter(&tool_view);
	// set tool view bit
	current_view |= Tool_View;
}

void MainWindow::hide_tool_view()
{
	tool_animation.setStartValue(tool_view.pos());
	tool_animation.setEndValue(QPoint(game_view.x(), -tool_view.height()));
	tool_animation.setDuration(ANIMATION_TIME);
	tool_animation.stop();
	tool_animation.start();

	// disable focus in tool view
	tool_view.disable_focus();
	this->setFocus();
	// disable events in tool view
	this->removeEventFilter(&tool_view);
	// clear tool view bit
	current_view &= ~Tool_View;
}

void MainWindow::show_help_view()
{
	// make it visible
	help_view.show();

	// hide tool view but keep bit set
	if(current_view & Tool_View)
	{
		tool_animation.setStartValue(tool_view.pos());
		tool_animation.setEndValue(QPoint(game_view.x(), -tool_view.height()));
		tool_animation.setDuration(ANIMATION_TIME);
		tool_animation.stop();
		tool_animation.start();

		// disable focus in tool view
		tool_view.disable_focus();
		this->setFocus();
		// disable events in tool view
		this->removeEventFilter(&tool_view);
	}

	if(current_view & Preferences_View)
	{
		// reduce preferences view
		preferences_animation.setStartValue(preferences_view.pos());
		preferences_animation.setEndValue(QPoint(help_view.width(), game_view.y()));
		preferences_animation.setDuration(ANIMATION_TIME);
		preferences_animation.stop();
		preferences_animation.start();
	}

	help_animation.setStartValue(help_view.pos());
	help_animation.setEndValue(QPoint(game_view.x(), game_view.y()));
	help_animation.setDuration(ANIMATION_TIME);
	help_animation.stop();
	help_animation.start();

	// enable focus in help view
	help_view.enable_focus();
	// enable events in help view
	this->installEventFilter(&help_view);
	// set help view bit
	current_view |= Help_View;
}

void MainWindow::hide_help_view()
{
	if(current_view & Preferences_View)
	{
		// extend preferences view
		preferences_animation.setStartValue(preferences_view.pos());
		preferences_animation.setEndValue(QPoint(width() - preferences_view.width(), game_view.y()));
		preferences_animation.setDuration(ANIMATION_TIME);
		preferences_animation.stop();
		preferences_animation.start();
	}
	// if preferences view is not active, but tool view
	else if(current_view & Tool_View)
		show_tool_view();

	help_animation.setStartValue(help_view.pos());
	help_animation.setEndValue(QPoint(-help_view.width(), game_view.y()));
	help_animation.setDuration(ANIMATION_TIME);
	help_animation.stop();
	help_animation.start();

	// disable focus in help view
	help_view.disable_focus();
	this->setFocus();
	// disable events in help view
	this->removeEventFilter(&help_view);
	// clear help view bit
	current_view &= ~Help_View;
}
