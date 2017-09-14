#ifndef LABEL_H
#define LABEL_H

#include "settingswindow.h"
#include <QWidget>
#include <QLabel>

class Label : public QLabel
{
	Q_OBJECT

	SettingsWindow* settings;

	unsigned int value;

public:
	Label(SettingsWindow* settings, unsigned int value = 0, QWidget* parent = 0);

public slots:
	void changeTimer(unsigned int newValue);
	void newTimer();
};

#endif // LABEL_H