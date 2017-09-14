#ifndef GRAPHICCELL_H
#define GRAPHICCELL_H

#include "cell.h"
#include "settingswindow.h"
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class GraphicCell : public QGraphicsItem
{
//	Q_OBJECT

	SettingsWindow* settings;

	Cell &cell;
	QRectF rect;

	unsigned int x;
	unsigned int y;

	unsigned int currentGridWidth;

	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

public:
	GraphicCell(Cell &cell, SettingsWindow *settings);
	~GraphicCell() = default;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	std::vector<std::vector<Cell> > nextGen();

	void kill();

	inline const unsigned int& getGeneration() { return this->cell.getGeneration(); }
	inline unsigned int& getOriginalGeneration() { return cell.getOriginalGeneration(); }

	static bool clicked;

	QRectF boundingRect() const;

	inline unsigned int getX() { return x; }
	inline unsigned int getY() { return y; }
};

#endif // GRAPHICCELL_H
