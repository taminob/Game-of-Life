// © Copyright (c) 2018 SqYtCO

#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QTextBrowser>
#include <QCheckBox>
#include <QToolButton>

// dialog which is shown after startup
// dialog can be disabled with the checkbox inside the dialog
// signals: show_help()
class StartupDialog : public QDialog
{
	Q_OBJECT

	// container of all comonents
	QGridLayout main_layout;
	// display welcome text
	QTextBrowser html_welcome;
	// enable/disable dialog on startup in preferences
	QCheckBox show_on_startup;
	// show help; emit on click show_help()
	QToolButton show_help_button;
	// close dialog
	QToolButton close_button;

	// setup GUI
	void init_gui();

public:
	// setup window and connect signals
	StartupDialog(QWidget* parent = nullptr);
	virtual ~StartupDialog() override = default;

	// set all texts; if a QTranslator is installed, the installed texts will be loaded
	void translate();

signals:
	// has to be connected to a slot which shows help
	void show_help();
};

#endif // STARTUPDIALOG_H
