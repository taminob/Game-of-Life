#include "graphiccell.h"

bool GraphicCell::clicked;

GraphicCell::GraphicCell(Cell &cell, SettingsWindow* settings) : cell(cell)
{
	this->settings = settings;
	x = this->settings->getCellSize() * cell.getX();
	y = this->settings->getCellSize() * cell.getY();
	rect = QRectF(QPointF(y, x), QSizeF(this->settings->getCellSize(), this->settings->getCellSize()));
	currentGridWidth = settings->getGridWidth();
	clicked = false;
}

QRectF GraphicCell::boundingRect() const
{
	return rect;
}

void GraphicCell::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event)

	clicked = true;

	if(event->buttons() == Qt::LeftButton)
	{
		cell.getOriginalAlive() = !(cell.isAlive());
	}
	else if(event->buttons() == Qt::RightButton)
	{
		cell.getOriginalAlive() = false;
	}
	this->update(rect);
}

void GraphicCell::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event)

	clicked = false;
}

void GraphicCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	if(!cell.isAlive())
		painter->setBrush(settings->getDeadCellColor());
	else
		painter->setBrush(settings->getAliveCellColor());

	painter->setPen(QPen(settings->getLineColor(), currentGridWidth));
	painter->drawRect(rect);
}

std::vector< std::vector<Cell> > GraphicCell::nextGen()
{
	return cell.nextGen();
}

void GraphicCell::kill()
{
	cell.getOriginalAlive() = false;
	cell.getOriginalGeneration() = 0;
}
