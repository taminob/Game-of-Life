#ifndef GRAPHICALWINDOW_H
#define GRAPHICALWINDOW_H

#include "cellsystem.h"
#include "graphiccell.h"
#include "cell.h"
#include "settings.h"
#include <QGraphicsView>

class QGraphicsScene;
class QMouseEvent;
class QMenu;

class GraphicalWindow : public QGraphicsView
{
	Q_OBJECT

	Settings* settings;

	QMenu* contextmenu;
	std::vector<GraphicCell*> gcells;

	unsigned int currentCellSize;

	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;

public:
	GraphicalWindow(Settings* settings, QGraphicsView* parent = nullptr);

	CellSystem system;
	QGraphicsScene* scene;

	void setContextMenu(QMenu* contextMenu);
	void setCellScene(QGraphicsScene* scene, const int& width, const int& height, const bool& createNewSystem = true);

	const CellSystem& createCells(int w, int h);

public slots:
	void showContextMenu(const QPoint& pos);
	void fullUpdate();
	void sceneUpdate();
	void clearAll();
};

#endif // GRAPHICALWINDOW_H
