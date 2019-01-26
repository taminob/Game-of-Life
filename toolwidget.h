// © Copyright (c) 2018 SqYtCO

#ifndef TOOLWIDGET_H
#define TOOLWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QSpinBox>
#include <QLabel>

// widget of tool view
// this widget is a toolbar which contains some useful tools
class ToolWidget : public QFrame
{
	Q_OBJECT

	// container of GUI components
	QHBoxLayout main_layout;

	// setup GUI and add components to main_layout
	void init_gui();
	// init preferences, help, hide and quit button
	void init_control_buttons();
	// init all other buttons
	void init_buttons();
	// init labels and spinbox
	void init_others();

	void play_or_stop();
	void swap_mouse_behavior();

public:
	// setup widget
	ToolWidget(QWidget* parent = nullptr);
	virtual ~ToolWidget() override = default;

	// compile time constants
	static constexpr std::size_t TOOL_HEIGHT = 35;
	static constexpr std::size_t MAXIMUM_GENERATIONS_PER_STEP_INPUT = 1000000;

	// called when autogenerating is started/stopped to update play_stop button
	void update_play_stop_button();
	// called when a new system is created to update current_size label
	void update_current_size_label();
	// called when colors have been changed
	void update_mouse_previews();

	// set all texts; if a QTranslator is installed, the installed texts will be loaded
	void translate();

	// enable the focus of all children and the widget
	void enable_focus();
	// disable the focus of all children and the widget
	void disable_focus();

protected:
	// receive events; pass all events to parent except mousePressEvent
	virtual bool eventFilter(QObject* watched, QEvent* event) override;
	// emit hide_tool() on press of 'Esc'
	virtual void keyPressEvent(QKeyEvent* event) override;

signals:
	// emitted on 'Esc'-key press and on hide button click; has to be connected to a slot which hides toolbar
	void hide_tool();
	// emitted on help button click; has to be connected to a slot which shows help
	void show_help();
	// emitted on preferences button click; has to be connected to a slot which shows preferences
	void show_preferences();
	// emitted on fullscreen button click; has to be connected to a slot which toggles fullscreen mode
	void fullscreen_changed();

// GUI components
private:
	QToolButton save;
	QToolButton open;
	QToolButton hide;
	QToolButton quit;
	QToolButton help;
	QToolButton preferences;
	QToolButton play_stop;
	QToolButton step;
	QSpinBox generations_per_step;
	QLabel right_mouse_preview;
	QLabel left_mouse_preview;
	QToolButton swap_mouse;
	QToolButton clear_all;
	QToolButton new_game;
	QToolButton reset_movement;
	QLabel current_size;
	QToolButton fullscreen;
};

#endif // TOOLWIDGET_H
