#include "separatethread.h"
#include <QThread>

QMutex SeparateThread::systemMutex;

SeparateThread::SeparateThread(GraphicalWindow* graphic, Settings* settings, Label* label, CellSystem& system, const int& loops, QObject* parent) : QObject(parent), loops(loops), autoplaySpeed(settings->getAutoplaySpeed()), system(system)
{
	QObject::connect(this, SIGNAL(changedTimer(uint)), label, SLOT(changeTimer(uint)));
	QObject::connect(this, SIGNAL(updateGraphic()), graphic, SLOT(sceneUpdate()));
	QObject::connect(settings, &Settings::autoplaySpeedChanged, [this, settings](){ this->autoplaySpeed = settings->getAutoplaySpeed(); } );
}

void SeparateThread::work()
{
	if(loops == -1)
	{
		while(loops != -2)
		{
			systemMutex.lock();
			system.nextGen();
			emit changedTimer(system.getGeneration());
			emit updateGraphic();
			systemMutex.unlock();
			QThread::msleep(autoplaySpeed);
		}
	}
	else if(loops == -3)
	{
		systemMutex.lock();
		system.nextGen();
		emit changedTimer(system.getGeneration());
		emit updateGraphic();
		systemMutex.unlock();
	}
	else
	{
		for(int i = 0; i < loops && loops > 0; ++i)
		{
			systemMutex.lock();
			system.nextGen();
			emit changedTimer(system.getGeneration());
			emit updateGraphic();
			systemMutex.unlock();
			QThread::msleep(autoplaySpeed);
		}
	}
}
