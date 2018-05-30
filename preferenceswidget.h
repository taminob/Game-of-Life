// © Copyright (c) 2018 SqYtCO

#ifndef PREFERENCESWIDGET_H
#define PREFERENCESWIDGET_H

#include "graphiccore.h"
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QLineEdit>

// see definition below
class RuleButton;

// widget of preferences view
// this widget contains all options to modify the application
// signals: hide_preferences(), language_changed(), color_changed()
class PreferencesWidget : public QFrame
{
	Q_OBJECT

	// contain all components
	QGridLayout main_layout;

	// call init functions and add groups to main layout
	void init_GUI();
	// init appearance group box and connect signals with slots
	void init_appearance_group();
	// init color group box and connect signals with slots
	void init_color_group();
	// init game group box and connect signals with slots
	void init_game_group();
	// init application group box and connect signals with slots
	void init_application_group();
	// set sizepolicy and connect signals with slots
	void init_control_buttons();
	// set icons, add buttons to layout and connect signals with slots
	void init_language_buttons();

	// if true, a new game has to be started to apply some changes
	bool restart_required;

	// reload preferences to input fields; these functions are called in reload()
	void reload_colors();
	void reload_game();
	void reload_appearance();
	void reload_application();

	// check if path is writable and set accordingly the style of save_path_input
	void check_save_path();

public:
	// setup GUI and components and load values
	PreferencesWidget(QWidget* parent = nullptr);
	virtual ~PreferencesWidget() = default;

	// enable the focus of all children and the widget
	void enable_focus();
	// disable the focus of all children and the widget
	void disable_focus();

	// call reload_xxxx() functions for groups to update changed preferences
	void reload();

	// set all texts; if a QTranslator is installed, the installed texts will be loaded
	void translate();

	static constexpr std::size_t PREFERENCES_WIDTH = 400;

signals:
	void hide_preferences();	// fired when hide-button is pressed; has to be connected with a slot which hides widget
	void language_changed();	// fired when any language-button is pressed; has to be connected with a slot which installs translators
	void color_changed();		// fired on color change; this signals is only for information purposes for other classes

public slots:
	void apply_changes();		// save changes
	void discard_changes();		// reload preferences
	void reset_to_default();	// reset changes to default values

protected:
	// receive events; pass all events to parent except mousePressEvent and 'Esc'-keyPressEvent
	virtual bool eventFilter(QObject*, QEvent* event) override;
	// implement shortcut: 'Esc' -> preferences_view
	virtual void keyPressEvent(QKeyEvent* event) override;

// functions for changing color; show QColorDialog and set color
private slots:
	void change_dead_color();
	void change_alive_color();
	void change_reviving_color();
	void change_dying_color();
	void change_background_color();

// GUI components
private:
	// control buttons
	QToolButton hide;
	QToolButton apply;
	QToolButton discard;
	QToolButton reset;

	// language buttons
	QHBoxLayout language_layout;
	QToolButton english;
	QToolButton german;

	// color preferences
	QGroupBox color_group;
	QGridLayout color_layout;
	QPushButton dead_color_change;
	QLabel dead_color_preview;
	QLabel dead_color_text;
	QPushButton alive_color_change;
	QLabel alive_color_preview;
	QLabel alive_color_text;
	QPushButton reviving_color_change;
	QLabel reviving_color_preview;
	QLabel reviving_color_text;
	QPushButton dying_color_change;
	QLabel dying_color_preview;
	QLabel dying_color_text;
	QPushButton background_color_change;
	QLabel background_color_preview;
	QLabel background_color_text;

	// appearance preferences
	QGroupBox appearance_group;
	QGridLayout appearance_layout;
	QCheckBox show_grid_check;
	QCheckBox show_generation_counter_check;
	QLabel generation_counter_size_text;
	QSpinBox generation_counter_size_input;
	QLabel generation_counter_size_unit;

	// game preferences
	QGroupBox game_group;
	QGridLayout game_layout;
	QLabel size_x_text;
	QSpinBox size_x_input;
	QLabel size_x_unit;
	QLabel size_y_text;
	QSpinBox size_y_input;
	QLabel size_y_unit;
	QLabel border_behavior_text;
	QComboBox border_behavior_input;
	QCheckBox random_start;
	QLabel relation_text;
	QLabel relation_alive_text;
	QLabel relation_dead_text;
	QSpinBox relation_dead_input;
	QSpinBox relation_alive_input;
	QLabel delay_between_generations_text;
	QSpinBox delay_between_generations_input;
	QLabel delay_between_generations_unit;
	QLabel live_rules_text;
	std::array<RuleButton*, 9> live_rules_input;
	QLabel reborn_rules_text;
	std::array<RuleButton*, 9> reborn_rules_input;
	QCheckBox lock_cells_after_generation_zero_check;

	// application preferences
	QGroupBox application_group;
	QGridLayout application_layout;
	QLabel num_of_threads_text;
	QSpinBox num_of_threads_input;
	QCheckBox start_fullscreen;
	QCheckBox show_exit_warning_check;
	QLabel save_path_text;
	QLineEdit save_path_input;
	QToolButton save_path_browse;
	QPushButton show_saved_games_dir;
};

// button which contains one num_of_neighbors number for live or reborn rule representation in preferences view
// button is down if the rule is active and up if the rule is inactive
class RuleButton : public QToolButton
{
	// true if rule is active, otherwise false
	bool state;

public:
	// setup appearance of button
	RuleButton(const std::size_t& num, QWidget* parent = nullptr) : QToolButton(parent), state(false)
	{
		this->setText(QString::number(num));
		this->setAutoRaise(true);
		this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	}

	// set state and update down-property
	void set_state(const bool& new_state)
	{
		state = new_state;
		this->setDown(state);
	}

	// toggle state and update down-property
	void toggle_state() { set_state(!state); }

	// return current state
	const bool& get_state() { return state; }
};

#endif // PREFERENCESWIDGET_H
