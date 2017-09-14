#include "graphicalwindow.h"
#include <QSpinBox>
#include <QStyleOptionGraphicsItem>

#include <random>
#include <ctime>

std::vector< std::vector<Cell> > GraphicalWindow::defaultVector;

GraphicalWindow::GraphicalWindow(SettingsWindow* settings, QGraphicsView *parent) : QGraphicsView(parent)
{
	this->settings = settings;

	this->setAutoFillBackground(true);
	this->setBackgroundBrush(settings->getBackgroundColor());

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

void GraphicalWindow::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::MidButton)
	{
		showContextMenu(event->pos());
	}
	else if(event->buttons() == Qt::RightButton)
	{
		QPointF posi = mapToScene(event->pos());

		if(posi.x() < scene->sceneRect().topLeft().x() && posi.y() < scene->sceneRect().topLeft().y() && posi.x() > scene->sceneRect().bottomRight().x() && posi.y() > scene->sceneRect().bottomRight().y())
		{
			showContextMenu(event->pos());
		}
	}

	QGraphicsView::mousePressEvent(event);
}

void GraphicalWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(GraphicCell::clicked)
	{
		QPointF posi = mapToScene(event->pos());

		if(posi.x() > scene->sceneRect().topLeft().x() && posi.y() > scene->sceneRect().topLeft().y() && posi.x() < scene->sceneRect().bottomRight().x() && posi.y() < scene->sceneRect().bottomRight().y())
		{
			if(event->buttons() == Qt::LeftButton)
				system[posi.y() / currentCellSize][posi.x() / currentCellSize].getOriginalAlive() = true;
			else if(event->buttons() == Qt::RightButton)
				system[posi.y() / currentCellSize][posi.x() / currentCellSize].getOriginalAlive() = false;
			scene->update(scene->sceneRect());
		}
	}

	QGraphicsView::mouseMoveEvent(event);
}

void GraphicalWindow::wheelEvent(QWheelEvent *event)
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

		this->ensureVisible(lf, tf, wf, hf, 0, 0);

		QPointF newPos = this->mapToScene(event->pos());

		this->ensureVisible(QRectF(QPointF(lf, tf) - newPos + pos, QSizeF(wf, hf)), 0, 0);

		event->accept();
	}
	else
	{
		QGraphicsView::wheelEvent(event);
	}

}

void GraphicalWindow::setCellScene(QGraphicsScene *scene, int x, int y, std::vector<std::vector<Cell> >& system)
{
	this->scene = scene;

	this->scene->clear();
	gcells.clear();

	currentCellSize = settings->getCellSize();
	this->scene->setSceneRect(QRectF(scene->sceneRect().x(), scene->sceneRect().y(), x * this->currentCellSize, y * this->currentCellSize));

	if(system.size() <= 0)
		createCells(x, y);

	for(auto &a : this->system)
	{
		for(auto &b : a)
		{
			gcells.push_back(new GraphicCell(b, settings));
		}
	}

	for(auto &a : gcells)
		this->scene->addItem(a);

	this->setScene(this->scene);
}

void GraphicalWindow::setContextMenu(QMenu* contextMenu)
{
	contextmenu = contextMenu;
//	this->setContextMenuPolicy(Qt::CustomContextMenu);
//	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));	// old contextmenu; new contextmenu: mouse wheel click
}

void GraphicalWindow::showContextMenu(const QPoint &pos)
{
   contextmenu->exec(mapToGlobal(pos));
}

const std::vector< std::vector<Cell> >& GraphicalWindow::createCells(int w, int h)
{

	system.clear();

	std::vector<Cell> row;

	qsrand(time(0));

	std::mt19937 mt(qrand());
	std::uniform_int_distribution<int> dist(0, 2);

	for(int a = 0; a < h; a++)
	{
		for(int b = 0; b < w; b++)
		{
			if(dist(mt))
				row.push_back(Cell(system, false, b, a, w - 1, h - 1));
			else
				row.push_back(Cell(system, true, b, a, w - 1, h - 1));
		}

		system.push_back(std::move(row));

		row.clear();

	}

	return system;
}

void GraphicalWindow::nextGraphicGen()
{
	this->gcells.back()->nextGen();
}

void GraphicalWindow::fullUpdate()
{
	for(auto a : gcells)
		a->update();
	this->scene->update(scene->sceneRect());
	this->update();
}

void GraphicalWindow::clearAll()
{
	for(auto a : gcells)
		a->kill();

	this->scene->update(scene->sceneRect());
}
