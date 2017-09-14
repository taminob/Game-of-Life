#ifndef SEPERATETHREAD_H
#define SEPERATETHREAD_H

#include "graphicalwindow.h"
#include "settingswindow.h"
#include "label.h"
#include <QThread>
#include <QMutex>
#include <QLabel>

class SeperateThread : public QObject
{
	Q_OBJECT

	QMutex mutex;

	GraphicalWindow* graphic;
	SettingsWindow* settings;
	QLabel* label;
	int &loops;

public:
	SeperateThread(GraphicalWindow* graphic, SettingsWindow* settings, Label* label, int &loops, QObject* parent = 0);
	~SeperateThread();

public slots:
	void work();

signals:
	void changedTimer(uint);
};

#endif // SEPERATETHREAD_H