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

	std::size_t x;
	std::size_t y;

	std::size_t currentGridWidth;

	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
	GraphicCell(Cell& cell, const std::size_t& x, const std::size_t& y, Settings* settings);

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	virtual QRectF boundingRect() const override;

	inline const std::size_t& getX() const { return x; }
	inline const std::size_t& getY() const { return y; }
};

#endif // GRAPHICCELL_H
