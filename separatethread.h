// © Copyright (c) 2017 SqYtCO

#ifndef SEPARATETHREAD_H
#define SEPARATETHREAD_H

#include "graphicalwindow.h"
#include "label.h"
#include <QObject>
#include <QMutex>

#include <vector>
#include <atomic>

class QThread;

class Settings;

class SeparateThread : public QObject
{
	Q_OBJECT

	const int& loops;
	std::atomic<int> autoplaySpeed;
	CellSystem& system;

public:
	SeparateThread(GraphicalWindow* graphic, Settings* settings, Label* label, CellSystem& system, const int& loops, QObject* parent = nullptr);

	static QMutex systemMutex;

public slots:
	void work();

signals:
	void changedTimer(uint);
	void updateGraphic();
};

#endif // SEPARATETHREAD_H
