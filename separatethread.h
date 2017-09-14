#ifndef SEPERATETHREAD_H
#define SEPERATETHREAD_H

#include "graphicalwindow.h"
#include "settingswindow.h"
#include "label.h"
#include <QThread>
#include <QMutex>
#include <QLabel>

class SeparateThread : public QObject
{
	Q_OBJECT

	QMutex mutex;

	GraphicalWindow* graphic;
	SettingsWindow* settings;
	QLabel* label;
	int &loops;

public:
	SeparateThread(GraphicalWindow* graphic, SettingsWindow* settings, Label* label, int &loops, QObject* parent = 0);
	~SeparateThread();

public slots:
	void work();

signals:
	void changedTimer(uint);
};

#endif // SEPERATETHREAD_H