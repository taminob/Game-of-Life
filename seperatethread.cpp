#include "seperatethread.h"
#include <QApplication>
#include <QMutex>

SeperateThread::SeperateThread(GraphicalWindow* graphic, SettingsWindow* settings, Label *label, int &loops, QObject *parent) : QObject(parent), graphic(graphic), settings(settings), label(label), loops(loops)
{
	QObject::connect(this, SIGNAL(changedTimer(uint)), label, SLOT(changeTimer(uint)));
}

SeperateThread::~SeperateThread()
{

}

void SeperateThread::work()
{
	if(loops == -1)
	{
		while(loops != -2)
		{
			mutex.lock();
			graphic->nextGraphicGen();
			emit changedTimer(graphic->getGeneration());
			graphic->scene->update(graphic->scene->sceneRect());
			QThread::msleep(settings->getAutoplaySpeed());
			mutex.unlock();
		}
	}
	else if(loops == -3)
	{
		mutex.lock();
		graphic->nextGraphicGen();
		emit changedTimer(graphic->getGeneration());
		graphic->scene->update(graphic->scene->sceneRect());
		loops = 3;
		mutex.unlock();
	}
	else
	{
		for(int i = 0; i < loops && loops > 0; ++i)
		{
			mutex.lock();
			graphic->nextGraphicGen();
			emit changedTimer(graphic->getGeneration());
			graphic->scene->update(graphic->scene->sceneRect());
			QThread::msleep(settings->getAutoplaySpeed());
			mutex.unlock();
		}
	}
}
