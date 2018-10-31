// © Copyright (c) 2018 SqYtCO

#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QFrame>
#include <QGridLayout>
#include <QTextBrowser>
#include <QToolButton>
#include <QLabel>

// widget of help view
// it displays in dependence of the set language the file "help_XX.html"; XX=language
// signals: hide_help()
class HelpWidget : public QFrame
{
	Q_OBJECT

	// container of all components
	QGridLayout main_layout;

	// html display
	QTextBrowser html_help;
	// emit signal hide_help()
	QToolButton hide;
	// jump to anchor "index"
	QToolButton show_index;

	void init_gui();

public:
	HelpWidget(QWidget* parent = nullptr);
	virtual ~HelpWidget() override = default;

	// set all texts; if a QTranslator is installed, the installed texts will be loaded
	void translate();

	// enable the focus of all children and the widget
	void enable_focus();
	// disable the focus of all children and the widget
	void disable_focus();

protected:
	// receive events; pass all events to parent except mousePressEvent, wheelEvent and 'Esc'-keyPressEvent
	virtual bool eventFilter(QObject*, QEvent* event) override;
	// implement shortcut: 'Esc' -> hide_help
	virtual void keyPressEvent(QKeyEvent* event) override;

signals:
	// has to be connected to a slot which hides the help widget
	void hide_help();
};

#endif // HELPWIDGET_H
