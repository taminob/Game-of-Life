#ifndef GRAPHICALWINDOW_H
#define GRAPHICALWINDOW_H

#include "graphiccell.h"
#include "cell.h"
#include "settingswindow.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QThread>
#include <QMouseEvent>
#include <QMenu>

#include <vector>

class GraphicalWindow : public QGraphicsView
{
	Q_OBJECT

	SettingsWindow* settings;

	QMenu* contextmenu;
	std::vector<GraphicCell*> gcells;
	std::vector< std::vector<Cell> > system;

	unsigned int currentCellSize;

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

public:
	GraphicalWindow(SettingsWindow* settings, QGraphicsView *parent = 0);
	~GraphicalWindow() = default;

	void setContextMenu(QMenu* contextMenu);
	QGraphicsScene* scene;
	void setCellScene(QGraphicsScene* scene, int x, int y, std::vector<std::vector<Cell> >& system = GraphicalWindow::defaultVector);

	const std::vector<std::vector<Cell> >& createCells(int w, int h);

	void nextGraphicGen();

	inline const unsigned int& getGeneration() { return gcells.back()->getGeneration(); }
	inline const std::vector< std::vector<Cell> >& getSystem() { return system; }
	inline std::vector< std::vector<Cell> >& getOriginalSystem() { return system; }

	static std::vector< std::vector<Cell> > defaultVector;

public slots:
	void showContextMenu(const QPoint &pos);
	void fullUpdate();
	void clearAll();
};

#endif // GRAPHICALWINDOW_H
