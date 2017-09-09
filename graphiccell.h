#ifndef GRAPHICCELL_H
#define GRAPHICCELL_H

#include "cell.h"
#include "settingswindow.h"
#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;
class QPainter;

class GraphicCell : public QGraphicsItem
{
	Settings* settings;

	Cell& cell;
	QRectF rect;

	unsigned int x;
	unsigned int y;

	unsigned int currentGridWidth;

	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
	GraphicCell(Cell& cell, const unsigned int& x, const unsigned int& y, Settings* settings);

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	virtual QRectF boundingRect() const override;

	inline const unsigned int& getX() const { return x; }
	inline const unsigned int& getY() const { return y; }
};

#endif // GRAPHICCELL_H
