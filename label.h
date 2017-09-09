#ifndef LABEL_H
#define LABEL_H

#include "settingswindow.h"
#include <QLabel>

class Label : public QLabel
{
	Q_OBJECT

	Settings* settings;

	unsigned int value;

public:
	Label(Settings* settings, unsigned int value = 0, QWidget* parent = nullptr);

public slots:
	void changeTimer(unsigned int newValue);
	void newTimer();
};

#endif // LABEL_H
