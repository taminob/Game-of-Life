// © Copyright (c) 2018 SqYtCO

#include "toolwidget.h"
#include "graphiccore.h"
#include <QApplication>		// get global keyboard modifier
#include <QKeyEvent>

ToolWidget::ToolWidget(QWidget* parent) : QFrame(parent)
{
	// grey transparent background
	this->setAutoFillBackground(true);
	this->setPalette(QPalette(QColor(0x40, 0x40, 0x40, 0xE9)));

	// dark border around widget
	this->setFrameShadow(QFrame::Raised);
	this->setFrameShape(QFrame::Panel);
	this->setLineWidth(2);

	init_gui();

	this->setFixedHeight(TOOL_HEIGHT);
}

bool ToolWidget::eventFilter(QObject*, QEvent* event)
{
	// handle filtered events
	switch(event->type())
	{
		case QEvent::KeyPress:
			keyPressEvent(static_cast<QKeyEvent*>(event));
			break;
		case QEvent::MouseButtonPress:
			// do not pass to parent
			return true;
		default:
			;
	}

	// pass event to parent
	return false;
}

void ToolWidget::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Escape)
		emit hide_tool();

	// pass to parent
	event->ignore();
}

void ToolWidget::init_gui()
{
	init_control_buttons();
	init_buttons();
	init_others();

	// add widgets in right order to layout
	main_layout.addWidget(&quit);
	main_layout.addStretch();
	main_layout.addWidget(&save);
	main_layout.addWidget(&open);
	main_layout.addStretch();
	main_layout.addWidget(&new_game);
	main_layout.addWidget(&clear_all);
	main_layout.addStretch();
	main_layout.addWidget(&play_stop);
	main_layout.addWidget(&step);
	main_layout.addWidget(&generations_per_step);
	main_layout.addStretch();
	main_layout.addWidget(&left_mouse_preview);
	main_layout.addWidget(&swap_mouse);
	main_layout.addWidget(&right_mouse_preview);
	main_layout.addStretch();
	main_layout.addWidget(&reset_movement);
	main_layout.addStretch();
	main_layout.addWidget(&current_size);
	main_layout.addStretch();
	main_layout.addWidget(&fullscreen);
	main_layout.addWidget(&preferences);
	main_layout.addWidget(&help);
	main_layout.addWidget(&hide);

	translate();

	main_layout.setMargin(1);


	// set layout of toolwidget
	this->setLayout(&main_layout);
}

void ToolWidget::init_control_buttons()
{
	// init preferences button
	preferences.setIcon(QIcon(":/images/preferences-90.png"));
	QObject::connect(&preferences, &QToolButton::clicked, [this]() { emit show_preferences(); });

	// init hide button
	hide.setArrowType(Qt::UpArrow);
	QObject::connect(&hide, &QToolButton::clicked, [this]() { emit hide_tool(); });

	// init help button
	help.setIcon(QIcon(":/images/help-90.png"));
	QObject::connect(&help, &QToolButton::clicked, [this]() { emit show_help(); });

	// connect quit button
	QObject::connect(&quit, &QToolButton::clicked, []() { qApp->closeAllWindows(); });
}

void ToolWidget::init_buttons()
{
	// connect save button; text will be set in translate()
	QObject::connect(&save, &QToolButton::clicked, []() { GraphicCore::write_save(); });

	// connect open button; text will be set in translate()
	QObject::connect(&open, &QToolButton::clicked, []() { GraphicCore::read_save(); });

	// init new button
	QObject::connect(&new_game, &QToolButton::clicked, [this]() { GraphicCore::new_system(); update_current_size_label(); });

	// init clear button: on click all cells will be dead; on click + CTRL all cells will be alive
	QObject::connect(&clear_all, &QToolButton::clicked, []()
	{
		if(QApplication::keyboardModifiers() == Qt::ControlModifier)
			GraphicCore::reset_cells(Alive);
		else
			GraphicCore::reset_cells(Dead);
	});

	// init play/stop button
	play_stop.setIcon(QIcon(":/images/play-90.png"));
	QObject::connect(&play_stop, &QToolButton::clicked, this, &ToolWidget::play_or_stop);

	// init reset_movement; text will be set in translate()
	QObject::connect(&reset_movement, &QToolButton::clicked, []() { GraphicCore::reset_movement(); } );

	// init button for swapping mouse button behavior in game view
	swap_mouse.setIcon(QIcon(":/images/exchange-90.png"));
	QObject::connect(&swap_mouse, &QToolButton::clicked, this, &ToolWidget::swap_mouse_behavior);

	// init button for going one step forward
	step.setIcon(QIcon(":/images/play-to-90.png"));
	QObject::connect(&step, &QToolButton::clicked, []() { GraphicCore::step(); });

	fullscreen.setIcon(QIcon(":/images/fullscreen-90.png"));
	QObject::connect(&fullscreen, &QToolButton::clicked, [this]() { emit fullscreen_changed(); });
}

void ToolWidget::init_others()
{
	// init left_mouse_preview; text will be set in translate()
	left_mouse_preview.setMaximumWidth(3 * height());
	left_mouse_preview.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	left_mouse_preview.setAlignment(Qt::AlignCenter);
	left_mouse_preview.setAutoFillBackground(true);

	// init right_mouse_preview; text will be set in translate()
	right_mouse_preview.setMaximumWidth(3 * height());
	right_mouse_preview.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	right_mouse_preview.setAlignment(Qt::AlignCenter);
	right_mouse_preview.setAutoFillBackground(true);
	if(GraphicCore::get_config()->get_left_button_alive_right_dead())
	{
		left_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_alive_color()));
		right_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_dead_color()));
	}
	else
	{
		left_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_dead_color()));
		right_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_alive_color()));
	}

	generations_per_step.setMaximum(MAXIMUM_GENERATIONS_PER_STEP_INPUT);
	generations_per_step.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
	generations_per_step.setMinimum(1);
	generations_per_step.setValue(static_cast<int>(GraphicCore::get_config()->get_generations_per_step()));
	generations_per_step.setCorrectionMode(QSpinBox::CorrectToNearestValue);
	generations_per_step.setMaximumWidth(static_cast<int>(2.5 * height()));
	QObject::connect(&generations_per_step, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int)
	{
		GraphicCore::get_config()->set_generations_per_step(static_cast<std::size_t>(generations_per_step.value()));
	});
	// after editing set focus to parent
	QObject::connect(&generations_per_step, &QSpinBox::editingFinished, [this]() { static_cast<QWidget*>(this->parent())->setFocus(); });

	current_size.setAlignment(Qt::AlignCenter);
}

void ToolWidget::translate()
{
	// control buttons
	new_game.setText(tr("New"));
	new_game.setToolTip(tr("New (N)"));
	clear_all.setText(tr("Clear"));
	clear_all.setToolTip(tr("Clear (A)/(CTRL+A)"));
	quit.setText(tr("Quit"));
	quit.setToolTip(tr("Quit (Q)"));
	save.setText(tr("Save"));
	save.setToolTip(tr("Save (S)"));
	open.setText(tr("Open"));
	open.setToolTip(tr("Open (O)"));
	hide.setToolTip(tr("Hide (Esc)"));
	preferences.setToolTip(tr("Preferences (P)"));
	help.setToolTip(tr("Help (H)"));

	// translate buttons
	update_play_stop_button();
	reset_movement.setText(tr("Reset Position"));
	reset_movement.setToolTip(tr("Reset Position (M)"));
	fullscreen.setToolTip(tr("Fullscreen (F)"));
	step.setToolTip(tr("Next Step (Spacebar)"));

	// translate other
	left_mouse_preview.setText(tr("Left"));
	left_mouse_preview.setToolTip(tr("Left Mouse Action"));
	right_mouse_preview.setText(tr("Right"));
	right_mouse_preview.setToolTip(tr("Right Mouse Action"));
	swap_mouse.setToolTip(tr("Swap Mouse Actions"));
	update_current_size_label();
	current_size.setToolTip(tr("Current Game Size"));
	generations_per_step.setToolTip(tr("Generations Per Step"));
}

void ToolWidget::update_play_stop_button()
{
	// if generating is running, set stop-icon
	if(GraphicCore::generating_running())
	{
		play_stop.setIcon(QIcon(":/images/stop-90.png"));
		play_stop.setToolTip(tr("Stop (R)"));
	}
	// if generation is stopped, set play-icon
	else
	{
		play_stop.setIcon(QIcon(":/images/play-90.png"));
		play_stop.setToolTip(tr("Play (R)"));
	}
}

void ToolWidget::update_current_size_label()
{
	current_size.setText(tr("Size: ") + QString::number(Core::get_size_x()) + "|" + QString::number(Core::get_size_y()));
}

void ToolWidget::update_mouse_previews()
{
	// update color of left_mouse/right_mouse
	if(GraphicCore::get_config()->get_left_button_alive_right_dead())
	{
		left_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_alive_color()));
		right_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_dead_color()));
	}
	else
	{
		left_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_dead_color()));
		right_mouse_preview.setPalette(QPalette(GraphicCore::get_config()->get_alive_color()));
	}
}

void ToolWidget::play_or_stop()
{
	// if generating is running, stop it and set play-icon
	if(GraphicCore::generating_running())
	{
		GraphicCore::stop_generating();
		play_stop.setIcon(QIcon(":/images/play-90.png"));
		play_stop.setToolTip(tr("Play (R)"));
	}
	// if generation is stopped, start it and set stop-icon
	else
	{
		GraphicCore::start_generating();
		play_stop.setIcon(QIcon(":/images/stop-90.png"));
		play_stop.setToolTip(tr("Stop (R)"));
	}
}

void ToolWidget::swap_mouse_behavior()
{
	// toggle left_button_alive_right_dead
	GraphicCore::get_config()->set_left_button_alive_right_dead(!GraphicCore::get_config()->get_left_button_alive_right_dead());

	update_mouse_previews();
}

void ToolWidget::enable_focus()
{
	// allow focus to all children
	for(QObject* a : this->children())
	{
		auto b = dynamic_cast<QWidget*>(a);
		if(b)
			b->setFocusPolicy(Qt::StrongFocus);
	}

	// enable own focus
	this->setFocusPolicy(Qt::StrongFocus);
}

void ToolWidget::disable_focus()
{
	// revoke focus of all children
	for(QObject* a : this->children())
	{
		auto b = dynamic_cast<QWidget*>(a);
		if(b)
			b->setFocusPolicy(Qt::NoFocus);
	}

	// disable own focus
	this->setFocusPolicy(Qt::NoFocus);
}
