// © Copyright (c) 2018 SqYtCO

#include "startupdialog.h"
#include "graphiccore.h"
#include <QLocale>

StartupDialog::StartupDialog(QWidget* parent) : QDialog(parent)
{
	// setup window min. size
	this->setMinimumSize(540, 420);
	// hide help button
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	// setup GUI
	init_gui();

	// load startup dialog preference
	show_on_startup.setChecked(GraphicCore::get_config()->get_show_startup_dialog());
	// connect checkbox
	QObject::connect(&show_on_startup, &QCheckBox::stateChanged, [this](bool state) { GraphicCore::get_config()->set_show_startup_dialog(state); } );
	// connect show help button
	QObject::connect(&show_help_button, &QToolButton::clicked, [this]() { emit show_help(); });
	// connect close button
	QObject::connect(&close_button, &QToolButton::clicked, [this]() { close(); });
}

void StartupDialog::translate()
{
	// dialog title
	this->setWindowTitle(tr("Welcome"));

	// at first startup German welcome will be displayed if system language is German; otherwise English
	if(GraphicCore::get_config()->get_language() == Language::German ||
			(GraphicCore::get_config()->get_language() == Language::System && QLocale::system().language() == QLocale::German))
		html_welcome.setSource(QUrl("qrc:/info/de_welcome.html"));
	else
		html_welcome.setSource(QUrl("qrc:/info/en_welcome.html"));

	// set button and checkbox texts
	close_button.setText(tr("Close"));
	show_help_button.setText(tr("Show Help"));
	show_on_startup.setText(tr("Show dialog on startup"));
}

void StartupDialog::init_gui()
{
	// allow internet and mail links
	html_welcome.setOpenExternalLinks(true);

	// init show_help button
	show_help_button.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

	// init close button
	close_button.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

	// add components to layout
	main_layout.addWidget(&html_welcome, 0, 0, 1, 2);
	main_layout.addWidget(&show_help_button, 1, 0, 1, 2);
	main_layout.addWidget(&show_on_startup, 2, 0);
	main_layout.addWidget(&close_button, 2, 1);

	// set texts
	translate();

	// set main layout
	this->setLayout(&main_layout);
}
