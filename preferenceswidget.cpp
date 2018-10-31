// © Copyright (c) 2018 SqYtCO

#include "preferenceswidget.h"
#include "core.h"
#include "graphiccore.h"
#include <QApplication>			// QApplication::style()
#include <QMessageBox>
#include <QKeyEvent>
#include <QColorDialog>			// QColorDialog::getColor()
#include <QFileDialog>			// QFileDialog::getExistingDirectory()
#include <QDesktopServices>		// QDesktopServices::openUrl()
#include <QDir>					// QDir::currentPath()
#include <thread>				// std::thread::hardware_concurrency()

PreferencesWidget::PreferencesWidget(QWidget* parent) : QFrame(parent), restart_required(false),
														survival_rules_input{{ new RuleButton(0, this), new RuleButton(1, this), new RuleButton(2, this),
																				new RuleButton(3, this), new RuleButton(4, this), new RuleButton(5, this),
																				new RuleButton(6, this), new RuleButton(7, this), new RuleButton(8, this) }},
														rebirth_rules_input{{ new RuleButton(0, this), new RuleButton(1, this), new RuleButton(2, this),
																				new RuleButton(3, this), new RuleButton(4, this), new RuleButton(5, this),
																				new RuleButton(6, this), new RuleButton(7, this), new RuleButton(8, this) }}
{
	// grey transparent background
	this->setAutoFillBackground(true);
	this->setPalette(QPalette(QColor(0x40, 0x40, 0x40, 0xE9)));

	// dark border around widget
	this->setFrameShadow(QFrame::Raised);
	this->setFrameShape(QFrame::Panel);
	this->setLineWidth(2);

	// setup gui
	init_gui();
}

bool PreferencesWidget::eventFilter(QObject*, QEvent* event)
{
	// handle filtered events
	switch(event->type())
	{
		case QEvent::KeyPress:
			keyPressEvent(static_cast<QKeyEvent*>(event));
			// return all keys except ESC-key
			if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape)
				return true;
			else
				return false;
		case QEvent::MouseButtonPress:
			// do not pass to parent
			return true;
		default:
			;
	}

	// pass event to parent
	return false;
}

void PreferencesWidget::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Escape)
		emit hide_preferences();

	// pass to parent
	event->ignore();
}

void PreferencesWidget::init_gui()
{
	init_color_group();
	init_game_group();
	init_appearance_group();
	init_application_group();
	init_language_buttons();
	init_control_buttons();

	// add buttons and groups to main layout
	main_layout.addWidget(&hide, 0, 0);
	main_layout.addLayout(&language_layout, 0, 1);
	main_layout.addWidget(&game_group, 1, 0, 4, 1);
	main_layout.addWidget(&appearance_group, 1, 1, 2, 1);
	main_layout.addWidget(&application_group, 3, 1, 2, 1);
	main_layout.addWidget(&color_group, 1, 2, 4, 1);
	main_layout.addWidget(&reset, 6, 0);
	main_layout.addWidget(&discard, 6, 1);
	main_layout.addWidget(&apply, 6, 2);

	// set texts
	translate();
	// load preferences
	reload();

	setLayout(&main_layout);
}

void PreferencesWidget::init_color_group()
{
	// text will be set in translate()
	// add texts to group layout
	color_layout.addWidget(&dead_color_text, 0, 0, 1, 2, Qt::AlignBottom);
	color_layout.addWidget(&alive_color_text, 3, 0, 1, 2, Qt::AlignBottom);
	color_layout.addWidget(&reviving_color_text, 6, 0, 1, 2, Qt::AlignBottom);
	color_layout.addWidget(&dying_color_text, 9, 0, 1, 2, Qt::AlignBottom);
	color_layout.addWidget(&background_color_text, 12, 0, 1, 2, Qt::AlignBottom);

	// init previews
	dead_color_preview.setAutoFillBackground(true);
	alive_color_preview.setAutoFillBackground(true);
	reviving_color_preview.setAutoFillBackground(true);
	dying_color_preview.setAutoFillBackground(true);
	background_color_preview.setAutoFillBackground(true);

	// add previews to group layout
	color_layout.addWidget(&dead_color_preview, 1, 0);
	color_layout.addWidget(&alive_color_preview, 4, 0);
	color_layout.addWidget(&reviving_color_preview, 7, 0);
	color_layout.addWidget(&dying_color_preview, 10, 0);
	color_layout.addWidget(&background_color_preview, 13, 0);

	// init change buttons
	QObject::connect(&dead_color_change, &QPushButton::clicked, this, &PreferencesWidget::change_dead_color);
	QObject::connect(&alive_color_change, &QPushButton::clicked, this, &PreferencesWidget::change_alive_color);
	QObject::connect(&reviving_color_change, &QPushButton::clicked, this, &PreferencesWidget::change_reviving_color);
	QObject::connect(&dying_color_change, &QPushButton::clicked, this, &PreferencesWidget::change_dying_color);
	QObject::connect(&background_color_change, &QPushButton::clicked, this, &PreferencesWidget::change_background_color);

	// add buttons to group layout
	color_layout.addWidget(&dead_color_change, 1, 1);
	color_layout.addWidget(&alive_color_change, 4, 1);
	color_layout.addWidget(&reviving_color_change, 7, 1);
	color_layout.addWidget(&dying_color_change, 10, 1);
	color_layout.addWidget(&background_color_change, 13, 1);

	// set group layout
	color_group.setLayout(&color_layout);
}

void PreferencesWidget::init_game_group()
{
	// init input of x/width
	size_x_input.setMinimum(1);
	size_x_input.setMaximum(99999);
	size_x_input.setCorrectionMode(QSpinBox::CorrectToNearestValue);
	QObject::connect(&size_x_input, &QSpinBox::editingFinished,
					 [this]() { Core::get_config()->set_size_x(size_x_input.value()); restart_required = true; });

	// init input of y/height
	size_y_input.setMinimum(1);
	size_y_input.setMaximum(99999);
	size_y_input.setCorrectionMode(QSpinBox::CorrectToNearestValue);
	QObject::connect(&size_y_input, &QSpinBox::editingFinished,
					 [this]() { Core::get_config()->set_size_y(size_y_input.value()); restart_required = true; });

	// create empty items and connect border behavior input; text will be set in translate()
	border_behavior_input.addItem("");
	border_behavior_input.addItem("");
	border_behavior_input.addItem("");
	border_behavior_input.addItem("");
	QObject::connect(&border_behavior_input, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this](int index)
	{
		// set only if choice is different from current border behavior
		if(Core::get_config()->get_border_behavior() != static_cast<Border_Behavior>(index))
		{
			Core::get_config()->set_border_behavior(static_cast<Border_Behavior>(index));
			restart_required = true;
		}
		enable_borderless_settings(static_cast<Border_Behavior>(index) == Border_Behavior::Borderless);
	});

	// init delay input
	delay_between_generations_input.setMinimum(1);
	delay_between_generations_input.setMaximum(10000);
	QObject::connect(&delay_between_generations_input, &QSpinBox::editingFinished,
					 [this]() { GraphicCore::get_config()->set_delay(delay_between_generations_input.value()); });

	// connect random start checkbox
	QObject::connect(&random_start, &QCheckBox::clicked, [this](int state)
	{
		Core::get_config()->set_start_random(state);

		if(state)
		{
			relation_alive_input.setEnabled(true);
			relation_dead_input.setEnabled(true);
		}
		else
		{
			relation_alive_input.setEnabled(false);
			relation_dead_input.setEnabled(false);
		}

		restart_required = true;
	});

	// init relation text
	relation_text.setAlignment(Qt::AlignCenter);
	relation_alive_text.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	relation_alive_text.setAlignment(Qt::AlignHCenter);
	relation_dead_text.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	relation_dead_text.setAlignment(Qt::AlignHCenter);
	QLabel* relation_between = new QLabel("<b>:</b>");
	relation_between->setAlignment(Qt::AlignCenter);
	relation_between->setMaximumWidth(50);

	// init relation input
	relation_alive_input.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QObject::connect(&relation_alive_input, &QSpinBox::editingFinished,
					 [this]() { Core::get_config()->set_relation_alive(relation_alive_input.value()); restart_required = true; });
	relation_dead_input.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QObject::connect(&relation_dead_input, &QSpinBox::editingFinished,
					 [this]() { Core::get_config()->set_relation_dead(relation_dead_input.value()); restart_required = true; });

	// create and init layout for random relation preferences
	QGridLayout* relation_layout = new QGridLayout;
	relation_layout->addWidget(&relation_text, 0, 0, 1, 3, Qt::AlignLeft | Qt::AlignBottom);
	relation_layout->addWidget(&relation_alive_text, 1, 0);
	relation_layout->addWidget(&relation_dead_text, 1, 2);
	relation_layout->addWidget(&relation_alive_input, 2, 0);
	relation_layout->addWidget(relation_between, 2, 1);
	relation_layout->addWidget(&relation_dead_input, 2, 2);

	// connect rulebuttons
	for(std::size_t i = 0; i < 9; ++i)
	{
		QObject::connect(survival_rules_input[i], &RuleButton::clicked, [this, i]()
		{
			// update state
			survival_rules_input[i]->toggle_state();
			// toggle survival_rule bit without changing other bits
			Core::get_config()->set_survival_rules(Core::get_config()->get_survival_rules() ^ (1 << i));

			restart_required = true;
		});
		QObject::connect(rebirth_rules_input[i], &RuleButton::clicked, [this, i]()
		{
			// update state
			rebirth_rules_input[i]->toggle_state();
			// toggle rebirth_rule bit without changing other bits
			Core::get_config()->set_rebirth_rules(Core::get_config()->get_rebirth_rules() ^ (1 << i));

			restart_required = true;
		});
	}
	// create layout and add rulebuttons
	QHBoxLayout* survival_rules_layout = new QHBoxLayout;
	QHBoxLayout* rebirth_rules_layout = new QHBoxLayout;
	for(std::size_t i = 0; i < 9; ++i)
	{
		survival_rules_layout->addWidget(survival_rules_input[i]);
		rebirth_rules_layout->addWidget(rebirth_rules_input[i]);
	}

	// connect lock cells checkbox
	QObject::connect(&lock_cells_after_generation_zero_check, &QCheckBox::clicked,
					 [this](bool state) { GraphicCore::get_config()->set_lock_after_first_generating(state); });

	// add components to group layout
	game_layout.addWidget(&size_x_text, 0, 0);
	game_layout.addWidget(&size_x_input, 0, 1);
	game_layout.addWidget(&size_x_unit, 0, 2);
	game_layout.addWidget(&size_y_text, 1, 0);
	game_layout.addWidget(&size_y_input, 1, 1);
	game_layout.addWidget(&size_y_unit, 1, 2);
	game_layout.addWidget(&border_behavior_text, 2, 0, 1, 1, Qt::AlignVCenter);
	game_layout.addWidget(&border_behavior_input, 2, 1, 1, 2, Qt::AlignVCenter);
	game_layout.addWidget(&random_start, 3, 0, 1, 3, Qt::AlignVCenter);
	game_layout.addLayout(relation_layout, 4, 0, 1, 3);
	game_layout.addWidget(&delay_between_generations_text, 5, 0, 1, 3, Qt::AlignBottom);
	game_layout.addWidget(&delay_between_generations_input, 6, 0, 1, 2);
	game_layout.addWidget(&delay_between_generations_unit, 6, 2, 1, 1);
	game_layout.addWidget(&survival_rules_text, 7, 0, 1, 3);
	game_layout.addLayout(survival_rules_layout, 8, 0, 1, 3);
	game_layout.addWidget(&rebirth_rules_text, 9, 0, 1, 3);
	game_layout.addLayout(rebirth_rules_layout, 10, 0, 1, 3);
	game_layout.addWidget(&lock_cells_after_generation_zero_check, 11, 0, 1, 3, Qt::AlignVCenter);

	// set group layout
	game_group.setLayout(&game_layout);
}

void PreferencesWidget::init_application_group()
{
	// init number of used threads input; set maximum to number of supported concurrent threads
	num_of_threads_input.setMinimum(1);
	num_of_threads_input.setMaximum(std::thread::hardware_concurrency());
	QObject::connect(&num_of_threads_input, &QSpinBox::editingFinished,
					 [this]() { Core::get_config()->set_num_of_threads(num_of_threads_input.value()); restart_required = true; });

	// connect fullscreen checkbox
	QObject::connect(&start_fullscreen, &QCheckBox::clicked,
					 [this](bool state) { GraphicCore::get_config()->set_fullscreen(state); });

	// connect exit warning checkbox
	QObject::connect(&show_exit_warning_check, &QCheckBox::clicked,
					 [this](bool state) { GraphicCore::get_config()->set_exit_warning(state); });


	// connect save path input
	QObject::connect(&save_path_input, &QLineEdit::textEdited, [this](const QString& input)
	{
		// if nothing is entered, return
		if(input.isEmpty())
			return;

		// non-const copy
		QString input_copy = input;

		// path must end with a separator
		if(input[input.size() - 1] != '/')
			input_copy.append('/');

		Core::get_config()->set_save_path(input_copy.toStdString());
		check_save_path();
	});
	// reload set path after editing
	QObject::connect(&save_path_input, &QLineEdit::editingFinished, [this]() { reload_application(); });

	// init browse for save path button
	save_path_browse.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
	QObject::connect(&save_path_browse, &QToolButton::clicked, [this]()
	{
		QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory..."), QString(Core::get_config()->get_save_path().c_str()));

		// if dialog is closed without OK
		if(dir.isEmpty())
			return;

		// path must end with a separator
		if(dir[dir.size() - 1] != '/')
			dir.append('/');

		Core::get_config()->set_save_path(dir.toStdString());
		reload_application();
	});

	// init show save dir; connect to openUrl which shows the directory in default file manager
	QObject::connect(&show_saved_games_dir, &QToolButton::clicked, [this]()
	{
		// complete default relative path to absolute path
		if(Core::get_config()->get_save_path() == Default_Values::SAVE_PATH)
			QDesktopServices::openUrl(QUrl(QDir::currentPath() + '/' +  QString(Core::get_config()->get_save_path().c_str())));
		else
			QDesktopServices::openUrl(QUrl(Core::get_config()->get_save_path().c_str()));
	});

	application_layout.addWidget(&num_of_threads_text, 0, 0);
	application_layout.addWidget(&num_of_threads_input, 0, 1);
	application_layout.addWidget(&start_fullscreen, 1, 0, 1, 3);
	application_layout.addWidget(&show_exit_warning_check, 2, 0, 1, 3);
	application_layout.addWidget(&save_path_text, 3, 0);
	application_layout.addWidget(&save_path_input, 3, 1);
	application_layout.addWidget(&save_path_browse, 3, 2);
	application_layout.addWidget(&show_saved_games_dir, 4, 1, 1, 2);

	application_group.setLayout(&application_layout);
}

void PreferencesWidget::init_appearance_group()
{
	// connect grid active checkbox
	QObject::connect(&show_grid_check, &QCheckBox::clicked,
					 [this](bool state) { GraphicCore::get_config()->set_grid_active(state); GraphicCore::update_opengl(); });

	// connect show generation counter checkbox
	QObject::connect(&show_generation_counter_check, &QCheckBox::clicked, [this](bool state)
	{
		GraphicCore::get_config()->set_hide_generation_counter(!state);
		if(!state)
			generation_counter_size_input.setEnabled(false);
		else
			generation_counter_size_input.setEnabled(true);
		GraphicCore::update_generation_counter();
	});

	// init generation counter size input
	generation_counter_size_input.setMinimum(1);
	generation_counter_size_input.setMaximum(999);
	QObject::connect(&generation_counter_size_input, &QSpinBox::editingFinished, [this]()
	{
		GraphicCore::get_config()->set_generation_counter_size(generation_counter_size_input.value());
		GraphicCore::update_generation_counter();
	});

	// add components to group layout
	appearance_layout.addWidget(&show_grid_check, 0, 0, 1, 2);
	appearance_layout.addWidget(&show_generation_counter_check, 1, 0, 1, 2);
	appearance_layout.addWidget(&generation_counter_size_text, 2, 0);
	appearance_layout.addWidget(&generation_counter_size_input, 2, 1);
	appearance_layout.addWidget(&generation_counter_size_unit, 2, 2);

	// set group layout
	appearance_group.setLayout(&appearance_layout);
}

void PreferencesWidget::init_control_buttons()
{
	// set sizepolicy and connect apply button
	apply.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QObject::connect(&apply, &QPushButton::clicked, this, &PreferencesWidget::apply_changes);
	QObject::connect(&apply, &QPushButton::clicked, [this]()
	{
		if(restart_required)
			QMessageBox::information(this, tr("New Game Required"), tr("Some changes require a new game to get applied."), QMessageBox::Ok);

		restart_required = false;
	});

	// set sizepolicy and connect discard button
	discard.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QObject::connect(&discard, &QPushButton::clicked, this, &PreferencesWidget::discard_changes);

	// set sizepolicy and connect reset button
	reset.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QObject::connect(&reset, &QPushButton::clicked, this, &PreferencesWidget::reset_to_default);

	// set arrow and sizepolicy and connect hide button (emit hide_preferences signal)
	hide.setArrowType(Qt::RightArrow);
	hide.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QObject::connect(&hide, &QToolButton::clicked, [this]() { emit hide_preferences(); });
}

void PreferencesWidget::init_language_buttons()
{
	// init english button (emit language_changed signal)
	english.setIcon(QIcon(":/images/usa-flag-90.png"));
	english.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QObject::connect(&english, &QToolButton::clicked, [this]()
	{
		GraphicCore::get_config()->set_language(Language::English);
		emit language_changed();
	});

	// init german button (emit language_changed signal)
	german.setIcon(QIcon(":/images/germany-flag-90.png"));
	german.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QObject::connect(&german, &QToolButton::clicked, [this]()
	{
		GraphicCore::get_config()->set_language(Language::German);
		emit language_changed();
	});

	// language buttons to layout
	language_layout.addWidget(&english);
	language_layout.addWidget(&german);
}

void PreferencesWidget::reload()
{
	reload_colors();
	reload_game();
	reload_appearance();
	reload_application();
}

void PreferencesWidget::reload_colors()
{
	QPalette temp;
	temp.setColor(QPalette::Background, GraphicCore::get_config()->get_dead_color());
	dead_color_preview.setPalette(temp);
	temp.setColor(QPalette::Background, GraphicCore::get_config()->get_alive_color());
	alive_color_preview.setPalette(temp);
	temp.setColor(QPalette::Background, GraphicCore::get_config()->get_reviving_color());
	reviving_color_preview.setPalette(temp);
	temp.setColor(QPalette::Background, GraphicCore::get_config()->get_dying_color());
	dying_color_preview.setPalette(temp);
	temp.setColor(QPalette::Background, GraphicCore::get_config()->get_background_color());
	background_color_preview.setPalette(temp);
}

void PreferencesWidget::reload_game()
{
	size_x_input.setValue(Core::get_config()->get_size_x());
	size_y_input.setValue(Core::get_config()->get_size_y());
	border_behavior_input.setCurrentIndex(static_cast<int>(Core::get_config()->get_border_behavior()));
	enable_borderless_settings(Core::get_config()->get_border_behavior() == Border_Behavior::Borderless);
	random_start.setChecked(Core::get_config()->get_start_random());
	if(Core::get_config()->get_start_random() && Core::get_config()->get_border_behavior() != Border_Behavior::Borderless)
	{
		relation_alive_input.setEnabled(true);
		relation_dead_input.setEnabled(true);
	}
	else
	{
		relation_alive_input.setEnabled(false);
		relation_dead_input.setEnabled(false);
	}
	relation_alive_input.setValue(Core::get_config()->get_relation_alive());
	relation_dead_input.setValue(Core::get_config()->get_relation_dead());
	for(std::size_t i = 0; i < 9; ++i)
	{
		survival_rules_input[i]->set_state(Core::get_config()->get_survival_rules() & (1 << i));
		rebirth_rules_input[i]->set_state(Core::get_config()->get_rebirth_rules() & (1 << i));
	}
	delay_between_generations_input.setValue(GraphicCore::get_config()->get_delay());
	lock_cells_after_generation_zero_check.setChecked(GraphicCore::get_config()->get_lock_after_first_generating());
}

void PreferencesWidget::reload_appearance()
{
	show_grid_check.setChecked(GraphicCore::get_config()->get_grid_active());
	show_generation_counter_check.setChecked(!GraphicCore::get_config()->get_hide_generation_counter());
	if(GraphicCore::get_config()->get_hide_generation_counter())
		generation_counter_size_input.setEnabled(false);
	else
		generation_counter_size_input.setEnabled(true);
	generation_counter_size_input.setValue(GraphicCore::get_config()->get_generation_counter_size());
}

void PreferencesWidget::reload_application()
{
	show_exit_warning_check.setChecked(GraphicCore::get_config()->get_exit_warning());
	start_fullscreen.setChecked(GraphicCore::get_config()->get_fullscreen());
	num_of_threads_input.setValue(Core::get_config()->get_num_of_threads());
	// only default path is relative
	if(Core::get_config()->get_save_path() == Default_Values::SAVE_PATH)
	{
		if(!QDir(Core::get_config()->get_save_path().c_str()).exists())
			QDir().mkdir(Core::get_config()->get_save_path().c_str());

		// set relative path
		save_path_input.setText(QDir::currentPath() + '/' +  QString(Core::get_config()->get_save_path().c_str()));
	}
	else
		// set absolute path
		save_path_input.setText(Core::get_config()->get_save_path().c_str());

	// check if save path is valid
	check_save_path();
}

void PreferencesWidget::translate()
{
	// control buttons: texts
	apply.setText(tr("Apply"));
	discard.setText(tr("Discard"));
	reset.setText(tr("Reset"));
	// tooltips
	apply.setToolTip(tr("Apply"));
	discard.setToolTip(tr("Discard"));
	reset.setToolTip(tr("Reset"));
	hide.setToolTip(tr("Hide (Esc)"));

	// language: tooltips
	english.setToolTip(tr("English"));
	german.setToolTip(tr("German"));

	// color: texts
	color_group.setTitle(tr("Color"));
	dead_color_text.setText(tr("Color Of Dead Cells"));
	alive_color_text.setText(tr("Color Of Alive Cells"));
	reviving_color_text.setText(tr("Color Of Reviving Cells"));
	dying_color_text.setText(tr("Color Of Dying Cells"));
	background_color_text.setText(tr("Color Of Background"));
	dead_color_change.setText(tr("Change"));
	alive_color_change.setText(tr("Change"));
	reviving_color_change.setText(tr("Change"));
	dying_color_change.setText(tr("Change"));
	background_color_change.setText(tr("Change"));
	// tooltips
	dead_color_text.setToolTip(tr("Color Of Dead Cells"));
	alive_color_text.setToolTip(tr("Color Of Alive Cells"));
	reviving_color_text.setToolTip(tr("Color Of Reviving Cells"));
	dying_color_text.setToolTip(tr("Color Of Dying Cells"));
	background_color_text.setToolTip(tr("Color Of Background"));
	dead_color_change.setToolTip(tr("Change"));
	alive_color_change.setToolTip(tr("Change"));
	reviving_color_change.setToolTip(tr("Change"));
	dying_color_change.setToolTip(tr("Change"));
	background_color_change.setToolTip(tr("Change"));

	// appearance: texts
	appearance_group.setTitle(tr("Appearance"));
	show_grid_check.setText(tr("Show Grid"));
	show_generation_counter_check.setText(tr("Show Generation Counter"));
	generation_counter_size_text.setText(tr("Size Of Generation Counter"));
	generation_counter_size_unit.setText(tr("px"));
	// tooltips
	show_grid_check.setToolTip(tr("Show Grid"));
	show_generation_counter_check.setToolTip(tr("Show Generation Counter"));
	generation_counter_size_text.setToolTip(tr("Size Of Generation Counter"));
	generation_counter_size_unit.setToolTip(tr("px"));

	// game: texts
	game_group.setTitle(tr("Game"));
	size_x_text.setText(tr("Width:"));
	size_x_unit.setText(tr("Cells"));
	size_y_text.setText(tr("Height:"));
	size_y_unit.setText(tr("Cells"));
	border_behavior_text.setText(tr("Behavior On Borders"));
	border_behavior_input.setItemText(0, tr("No Borders [Hashlife]"));
	border_behavior_input.setItemText(1, tr("Continue On Opposite Side"));
	border_behavior_input.setItemText(2, tr("Surround Board With Dead Cells"));
	border_behavior_input.setItemText(3, tr("Surround Board With Living Cells"));
	delay_between_generations_text.setText(tr("Autogenererating Delay"));
	delay_between_generations_unit.setText(tr("ms"));
	random_start.setText(tr("Fill New Game With Random Cells"));
	relation_text.setText(tr("Relation Of Random Generated Cells"));
	relation_alive_text.setText(tr("Alive"));
	relation_dead_text.setText(tr("Dead"));
	survival_rules_text.setText(tr("Number Of Neighbors Which Are Needed To Survive"));
	rebirth_rules_text.setText(tr("Number Of Neighbors Which Are Needed To Be Born"));
	lock_cells_after_generation_zero_check.setText(tr("Lock Cells After Initial Generation"));
	// tooltips
	size_x_text.setToolTip(tr("Width:"));
	size_x_unit.setToolTip(tr("Cells"));
	size_y_text.setToolTip(tr("Height:"));
	size_y_unit.setToolTip(tr("Cells"));
	border_behavior_text.setToolTip(tr("Behavior On Borders"));
	border_behavior_input.setItemData(0, tr("Continue On Opposite Side"), Qt::ToolTipRole);
	border_behavior_input.setItemData(1, tr("Surround Board With Dead Cells"), Qt::ToolTipRole);
	border_behavior_input.setItemData(2, tr("Surround Board With Living Cells"), Qt::ToolTipRole);
	delay_between_generations_text.setToolTip(tr("Autogenererating Delay"));
	delay_between_generations_unit.setToolTip(tr("ms"));
	random_start.setToolTip(tr("Fill New Game With Random Cells"));
	relation_text.setToolTip(tr("Relation Of Random Generated Cells"));
	relation_alive_text.setToolTip(tr("Alive"));
	relation_dead_text.setToolTip(tr("Dead"));
	survival_rules_text.setToolTip(tr("Number Of Neighbors Which Are Needed To Survive"));
	rebirth_rules_text.setToolTip(tr("Number Of Neighbors Which Are Needed To Be Born"));
	lock_cells_after_generation_zero_check.setToolTip(tr("Lock Cells After Initial Generation"));

	// application: texts
	application_group.setTitle(tr("Application"));
	show_saved_games_dir.setText(tr("Show Saved Game Directory"));
	show_exit_warning_check.setText(tr("Show Exit Warning"));
	start_fullscreen.setText(tr("Start In Fullscreen Mode"));
	num_of_threads_text.setText(tr("Number Of Threads Used For Calculation"));
	save_path_text.setText(tr("Default Path Of Saved Games"));
	// tooltips
	show_saved_games_dir.setToolTip(tr("Show Saved Game Directory"));
	show_exit_warning_check.setToolTip(tr("Show Exit Warning"));
	start_fullscreen.setToolTip(tr("Start In Fullscreen Mode"));
	num_of_threads_text.setToolTip(tr("Number Of Threads Used For Calculation"));
	save_path_text.setToolTip(tr("Default Path Of Saved Games"));
}

void PreferencesWidget::apply_changes()
{
	// if writing config fails
	if(!(Core::get_config()->write_config() && GraphicCore::get_config()->write_config()))
		QMessageBox::warning(this, tr("Write Error"), tr("Writing Configuration Failed!\nPlease Check Your Permissions."));

	GraphicCore::update_opengl();
	GraphicCore::update_generation_counter();
}

void PreferencesWidget::discard_changes()
{
	// read in saved preferences again
	Core::get_config()->read_config();
	GraphicCore::get_config()->read_config();

	// update GUI
	reload();
	GraphicCore::update_opengl();
	GraphicCore::update_generation_counter();
}

void PreferencesWidget::reset_to_default()
{
	// verify reset by user
	if(QMessageBox::warning(this, tr("Reset Preferences?"), tr("These Changes Can Not Be Undone!\nReset Preferences To Default?"), QMessageBox::Yes | QMessageBox::No)
			!= QMessageBox::Yes)
		return;

	// set config to default
	Core::get_config()->reset_config();
	GraphicCore::get_config()->reset_config();
	// update values in preferences view
	reload();

	emit color_changed();
	emit language_changed();

	// apply reset
	apply_changes();
	QMessageBox::information(this, tr("New Game Required"), tr("Some changes may require a new game to get applied."), QMessageBox::Ok);
}

void PreferencesWidget::check_save_path()
{
	// init save_path_default_palette
	static QPalette save_path_default_palette = save_path_input.palette();
	// check if write access is granted
	QFileInfo fileinfo(Core::get_config()->get_save_path().c_str());
	// if dir is writable
	if(fileinfo.isWritable() && fileinfo.isDir())
		// set color to default
		save_path_input.setPalette(save_path_default_palette);
	else
		// if write permission is granted, change color of input
		save_path_input.setPalette(QPalette(Qt::red));
}

void PreferencesWidget::enable_borderless_settings(bool enable)
{
	size_x_input.setDisabled(enable);
	size_y_input.setDisabled(enable);
	random_start.setDisabled(enable);
	relation_alive_input.setDisabled(enable);
	relation_dead_input.setDisabled(enable);
}

void PreferencesWidget::enable_focus()
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

void PreferencesWidget::disable_focus()
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

void PreferencesWidget::change_dead_color()
{
	QColor temp = QColorDialog::getColor(GraphicCore::get_config()->get_dead_color(), this, tr("Choose Color..."));

	if(temp.isValid())
	{
		GraphicCore::get_config()->set_dead_color(temp);
		reload_colors();
		GraphicCore::update_opengl();
		emit color_changed();
	}
}

void PreferencesWidget::change_alive_color()
{
	QColor temp = QColorDialog::getColor(GraphicCore::get_config()->get_alive_color(), this, tr("Choose Color..."));

	if(temp.isValid())
	{
		GraphicCore::get_config()->set_alive_color(temp);
		reload_colors();
		GraphicCore::update_opengl();
		emit color_changed();
	}
}

void PreferencesWidget::change_reviving_color()
{
	QColor temp = QColorDialog::getColor(GraphicCore::get_config()->get_reviving_color(), this, tr("Choose Color..."));

	if(temp.isValid())
	{
		GraphicCore::get_config()->set_reviving_color(temp);
		reload_colors();
		GraphicCore::update_opengl();
		emit color_changed();
	}
}

void PreferencesWidget::change_dying_color()
{
	QColor temp = QColorDialog::getColor(GraphicCore::get_config()->get_dying_color(), this, tr("Choose Color..."));

	if(temp.isValid())
	{
		GraphicCore::get_config()->set_dying_color(temp);
		reload_colors();
		GraphicCore::update_opengl();
		emit color_changed();
	}
}

void PreferencesWidget::change_background_color()
{
	QColor temp = QColorDialog::getColor(GraphicCore::get_config()->get_background_color(), this, tr("Choose Color..."));

	if(temp.isValid())
	{
		GraphicCore::get_config()->set_background_color(temp);
		reload_colors();
		GraphicCore::update_opengl();
		emit color_changed();
	}
}
