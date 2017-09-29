// © Copyright (c) 2017 SqYtCO

#ifndef LABEL_H
#define LABEL_H

#include "settingswindow.h"
#include <QLabel>

class Label : public QLabel
{
	Q_OBJECT

	Settings* settings;

	std::size_t value;

public:
	Label(Settings* settings, std::size_t value = 0, QWidget* parent = nullptr);

public slots:
	void changeTimer(const unsigned int& newValue);
	void newTimer();
};

#endif // LABEL_H
