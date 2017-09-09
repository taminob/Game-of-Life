#include "graphiccell.h"
#include "separatethread.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

GraphicCell::GraphicCell(Cell& cell, const unsigned int& x, const unsigned int& y, Settings* settings) : cell(cell)
{
	this->settings = settings;
	this->x = this->settings->getCellSize() * x;
	this->y = this->settings->getCellSize() * y;
	rect = QRectF(QPointF(this->y, this->x), QSizeF(this->settings->getCellSize(), this->settings->getCellSize()));
	currentGridWidth = settings->getGridWidth();
}

QRectF GraphicCell::boundingRect() const
{
	return rect;
}

void GraphicCell::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	Q_UNUSED(event)

	SeparateThread::systemMutex.lock();
	if(event->buttons() == Qt::LeftButton)
	{
		cell.alive = !cell.alive;			// inverts state
	}
	else if(event->buttons() == Qt::RightButton)
	{
		cell.alive = false;
	}
	SeparateThread::systemMutex.unlock();
	this->update(rect);
}

void GraphicCell::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	if(!cell.alive)
	{
		if(cell.next)
			painter->setBrush(settings->getNextGenAliveCellColor());
		else
			painter->setBrush(settings->getDeadCellColor());
	}
	else
	{
		if(!cell.next)
			painter->setBrush(settings->getNextGenDeadCellColor());
		else
			painter->setBrush(settings->getAliveCellColor());
	}

	painter->setPen(QPen(settings->getLineColor(), currentGridWidth));
	painter->drawRect(rect);
}
