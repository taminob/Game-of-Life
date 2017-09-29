// © Copyright (c) 2017 SqYtCO

#include "graphicalwindow.h"
#include "separatethread.h"
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QMenu>

#include <random>
#include <ctime>

GraphicalWindow::GraphicalWindow(Settings* settings, QGraphicsView* parent) : QGraphicsView(parent)
{
	this->settings = settings;

	this->setAutoFillBackground(true);
	this->setBackgroundBrush(settings->getBackgroundColor());
	this->setOptimizationFlag(QGraphicsView::DontSavePainterState);
	this->setCacheMode(QGraphicsView::CacheBackground);

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	if(settings->isScrollBarEnabled() == 0)
	{
		this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else if(settings->isScrollBarEnabled() == 1)
	{
		this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else
	{
		this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}

void GraphicalWindow::mousePressEvent(QMouseEvent* event)
{
	if(event->buttons() == Qt::MidButton)
	{
		showContextMenu(event->pos());
	}
	else if(event->buttons() == Qt::RightButton)
	{
		QPointF posi = mapToScene(event->pos());

		if(posi.x() < scene->sceneRect().topLeft().x() || posi.y() < scene->sceneRect().topLeft().y() ||
			posi.x() > scene->sceneRect().bottomRight().x() || posi.y() > scene->sceneRect().bottomRight().y())
		{
			showContextMenu(event->pos());
		}
	}

	QGraphicsView::mousePressEvent(event);
}

void GraphicalWindow::mouseMoveEvent(QMouseEvent* event)
{
	QPointF posi = mapToScene(event->pos());

	if(posi.x() > scene->sceneRect().topLeft().x() && posi.y() > scene->sceneRect().topLeft().y() &&
		posi.x() < scene->sceneRect().bottomRight().x() && posi.y() < scene->sceneRect().bottomRight().y())
	{
		SeparateThread::systemMutex.lock();
		if(event->buttons() == Qt::LeftButton)
			system[posi.y() / currentCellSize][posi.x() / currentCellSize].alive = true;
		else if(event->buttons() == Qt::RightButton)
			system[posi.y() / currentCellSize][posi.x() / currentCellSize].alive = false;
		SeparateThread::systemMutex.unlock();
		scene->update(scene->sceneRect());
	}

	QGraphicsView::mouseMoveEvent(event);
}

void GraphicalWindow::mouseReleaseEvent(QMouseEvent* event)
{
	SeparateThread::systemMutex.lock();
	system.calcNextGen();
	SeparateThread::systemMutex.unlock();

	scene->update(scene->sceneRect());
	QGraphicsView::mouseReleaseEvent(event);
}

void GraphicalWindow::wheelEvent(QWheelEvent* event)
{
	if(event->modifiers() == Qt::ControlModifier)
	{
		QPointF pos = this->mapToScene(event->pos());

		double by;									// factor of enlargement
		double angle = event->angleDelta().y();		// rotation of mouse wheel

		if(angle > 0)								// zoom in
			by = 1 + (angle / 360 * 0.3/*settings->getZoomFactor()*/);		// new feature
		else if(angle < 0)							// zoom out
			by = 1 - (-angle / 360 * 0.3/*settings->getZoomFactor()*/);		// new feature
		else										// nothing
			by = 1;

		this->scale(by, by);

		double w = this->viewport()->width();
		double h = this->viewport()->height();

		double wf = this->mapToScene(QPoint(w-1, 0)).x() - this->mapToScene(QPoint(0,0)).x();
		double hf = this->mapToScene(QPoint(0, h-1)).y() - this->mapToScene(QPoint(0,0)).y();

		double lf = pos.x() - event->pos().x() * wf / w;
		double tf = pos.y() - event->pos().y() * hf / h;

		QPointF newPos = this->mapToScene(event->pos());

		this->ensureVisible(QRectF(QPointF(lf, tf) - newPos + pos, QSizeF(wf, hf)), 0, 0);

		event->accept();
	}
	else
	{
		QGraphicsView::wheelEvent(event);
	}

}

void GraphicalWindow::setCellScene(QGraphicsScene* scene, const std::size_t& width, const std::size_t& height, const bool& createNewSystem)
{
	this->scene = scene;

	this->scene->clear();

	currentCellSize = settings->getCellSize();
	this->scene->setSceneRect(QRectF(scene->sceneRect().x(), scene->sceneRect().y(), width * this->currentCellSize, height * this->currentCellSize));

	SeparateThread::systemMutex.lock();
	if(createNewSystem)
		createCells(width, height);

	for(std::size_t a = 0; a < this->system.size(); ++a)
	{
		for(std::size_t b = 0; b < this->system[0].size(); ++b)
		{
			this->scene->addItem(new GraphicCell(this->system[a][b], a, b, settings));
		}
	}

	SeparateThread::systemMutex.unlock();


	this->setScene(this->scene);
}

void GraphicalWindow::setContextMenu(QMenu* contextMenu)
{
	contextmenu = contextMenu;		// mouse wheel (mid-button) click
}

void GraphicalWindow::showContextMenu(const QPoint& pos)
{
   contextmenu->exec(mapToGlobal(pos));
}

const CellSystem& GraphicalWindow::createCells(const std::size_t& w, const std::size_t& h)
{
	system.clear();

	std::vector<Cell> row;

	qsrand(time(0));

	std::mt19937 mt(qrand());
	std::uniform_int_distribution<int> dist(0, 2);

	system.reserve(h * w);

	for(std::size_t a = 0; a < h; a++)
	{
		row.reserve(w);
		for(std::size_t b = 0; b < w; b++)
		{
			if(dist(mt))
				row.push_back(Cell(false));
			else
				row.push_back(Cell(true));
		}

		system.push_back(std::move(row));

		row.clear();
	}

	system.setGeneration(0);
	system.calcNextGen();

	return system;
}

void GraphicalWindow::fullUpdate()
{
	this->scene->update(scene->sceneRect());
	this->update();
}

void GraphicalWindow::sceneUpdate()
{
	this->scene->update(this->scene->sceneRect());
}

void GraphicalWindow::clearAll()
{
	system.killAll();

	this->scene->update(scene->sceneRect());
}
