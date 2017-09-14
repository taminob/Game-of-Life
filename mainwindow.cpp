#include "mainwindow.h"
#include <QMessageBox>
#include <QFont>
#include <QPalette>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	mainwidget = new QWidget;
	settwin = new SettingsWindow;

	this->setWindowIcon(QIcon("game_of_life.ico"));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
	createContextMenu();

	grid = new QGridLayout;

	graphic = new GraphicalWindow(settwin);
	graphic->setContextMenu(contextmenu);
	scene = new QGraphicsScene(this);
	graphic->setCellScene(scene, settwin->getX(), settwin->getY());
	generationTimer = new Label(settwin, graphic->getGeneration(), this);
	graphic->stackUnder(generationTimer);
	grid->addWidget(graphic, 0, 0, 2, 2);
	grid->addWidget(generationTimer, 1, 1);
	mainwidget->setLayout(grid);
	this->setCentralWidget(mainwidget);

	createMenu();

	if(settwin->isFixed())
	{
		this->setFixedSize(QSize(settwin->getFixedWidth(), settwin->getFixedHeight()));
		this->show();
	}
	else
	{
		customShow();
	}
	if(settwin->isMenuHidden())
	{
		this->hidemenu(true);
	}
	else
	{
		showmenu();
	}

	settwin->setGraphicalWidgets(graphic, generationTimer);

	secondThread = new QThread(this);
	autoplayBreak = -3;
	worker = new SeperateThread(graphic, settwin, generationTimer, autoplayBreak);
	worker->moveToThread(secondThread);
	QObject::connect(this, SIGNAL(startThread()), worker, SLOT(work()));
	secondThread->start();
}

MainWindow::~MainWindow()
{
	secondThread->exit(0);

	delete menubar;
	delete settwin;
	delete worker;
	delete secondThread;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(settwin->isExitWarningEnabled())
	{
		QMessageBox::StandardButton btn = QMessageBox::question(this, "Exit", "Do you really want to exit?", QMessageBox::Yes | QMessageBox::No);

		if(btn == QMessageBox::Yes)
		{
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
}

void MainWindow::createMenu()
{
	menubar = new QMenuBar;

	start = menubar->addMenu(tr("&Start"));
	view = menubar->addMenu(tr("&View"));
	settings = menubar->addMenu(tr("Se&ttings"));
	help = menubar->addMenu(tr("&Help"));

	createSubMenu();

	this->setMenuBar(menubar);
}

void MainWindow::createSubMenu()
{
	startnew = start->addAction(tr("&New"));
	startnew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
	QObject::connect(startnew, SIGNAL(triggered(bool)), this, SLOT(startNew()));
	this->addAction(startnew);
	open = start->addAction(tr("&Open"));
	open->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	QObject::connect(open, SIGNAL(triggered(bool)), this, SLOT(openGame()));
	this->addAction(open);
	save = start->addAction(tr("&Save"));
	save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	QObject::connect(save, SIGNAL(triggered(bool)), this, SLOT(saveGame()));
	this->addAction(save);
	clearall = start->addAction(tr("&Clear"));
	clearall->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	QObject::connect(clearall, SIGNAL(triggered(bool)), this, SLOT(clearAll()));
	this->addAction(clearall);
	autoplays = start->addAction(tr("&Auto Play"));
	autoplays->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	QObject::connect(autoplays, SIGNAL(triggered(bool)), this, SLOT(autoplay()));
	this->addAction(autoplays);
	resumes = start->addAction(tr("&Resume"));
	resumes->setShortcut(QKeySequence(Qt::Key_Space));
	QObject::connect(resumes, SIGNAL(triggered(bool)), this, SLOT(resume()));
	this->addAction(resumes);
	pauses = start->addAction(tr("&Pause"));
	pauses->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	QObject::connect(pauses, SIGNAL(triggered(bool)), this, SLOT(pause()));
	this->addAction(pauses);
	QAction* updateScene = start->addAction(tr("&Update"));
	QObject::connect(updateScene, SIGNAL(triggered(bool)), this, SLOT(updates()));

	start->addSeparator();
	QAction* startExit = start->addAction(tr("E&xit"));
//	startExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
	startExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	QObject::connect(startExit, SIGNAL(triggered(bool)), this, SLOT(close()));
	this->addAction(startExit);

	hideMenu = view->addAction(tr("&Hide Menu"));
	hideMenu->setCheckable(true);
	hideMenu->setChecked(settwin->isMenuHidden());
	hideMenu->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
	QObject::connect(hideMenu, SIGNAL(triggered(bool)), this, SLOT(hidemenu(bool)));
	this->addAction(hideMenu);
	view->addSeparator();
	QAction* minimize = view->addAction(tr("M&inimize"));
	QObject::connect(minimize, SIGNAL(triggered(bool)), this, SLOT(showMinimized()));
	maximize = view->addAction(tr("&Maximize"));
	maximize->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	QObject::connect(maximize, SIGNAL(triggered(bool)), this, SLOT(showMaximized()));
	this->addAction(maximize);
	fullscreen = view->addAction(tr("&Fullscreen"));
	fullscreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
	QObject::connect(fullscreen, SIGNAL(triggered(bool)), this, SLOT(showFullScreen()));
	this->addAction(fullscreen);

	QAction* generalSetting = settings->addAction(tr("&General"));
	QObject::connect(generalSetting, SIGNAL(triggered(bool)), settwin, SLOT(showGeneral()));
	QAction* gameSetting = settings->addAction(tr("G&ame"));
	QObject::connect(gameSetting, SIGNAL(triggered(bool)), settwin, SLOT(showGame()));
	settings->addSeparator();
	setting = settings->addAction(tr("&Settings"));
	setting->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	QObject::connect(setting, SIGNAL(triggered(bool)), settwin, SLOT(show()));
	this->addAction(setting);

	help1 = help->addAction(tr("&Help"));
	help1->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
	QObject::connect(help1, SIGNAL(triggered(bool)), this, SLOT(helpWindow()));
	this->addAction(help1);
	help->addSeparator();
	QAction* aboutQt = help->addAction(tr("About Qt"));
	QObject::connect(aboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
	QAction* aboutAction = help->addAction(tr("About"));
	QObject::connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
}

void MainWindow::startNew()
{
	pause();

	graphic->setCellScene(scene, settwin->getX(), settwin->getY());
	generationTimer->newTimer();
}

void MainWindow::clearAll()
{
	pause();

	graphic->clearAll();
	generationTimer->changeTimer(0);
}

void MainWindow::autoplay()
{
	autoplayBreak = settwin->getMaxAutoplay();
	emit startThread();
}

void MainWindow::pause()
{
	autoplayBreak = -2;
}

void MainWindow::resume()
{
	autoplayBreak = -3;

	emit startThread();
}

void MainWindow::saveGame()
{
	settwin->saveGame();
}

void MainWindow::openGame()
{
	qDebug("open . . .");
	settwin->loadGame();
}

void MainWindow::updates()
{
	graphic->fullUpdate();
	settwin->loadSettings();
	settwin->updateSettings();
}

void MainWindow::hidemenu(bool activated)
{
	if(activated)
	{
		menubar->hide();
		hideMenu->setChecked(true);
		hideMenu2->setChecked(true);
	}
	else
	{
		showmenu();
	}
}

void MainWindow::showmenu()
{
	menubar->show();
	hideMenu->setChecked(false);
	hideMenu2->setChecked(false);
}

void MainWindow::customShow()
{
	if(settwin->getWindowStartmodus() == 0)
	{
		this->show();
	}
	else if(settwin->getWindowStartmodus() == 1)
	{
		this->showMaximized();
	}
	else if(settwin->getWindowStartmodus() == 2)
	{
		this->showMinimized();
	}
	else if(settwin->getWindowStartmodus() == 3)
	{
		this->showFullScreen();
	}
}

void MainWindow::helpWindow()
{
	QMessageBox::information(this, "Help", "Here you can get help!");
}

void MainWindow::about()
{
	QMessageBox::about(this, "About", "Me");
}

void MainWindow::showContextMenu(const QPoint &pos)
{	
   contextmenu->exec(mapToGlobal(pos));
}

void MainWindow::createContextMenu()
{
	contextmenu = new QMenu(tr("Context Menu"), this);

	hideMenu2 = contextmenu->addAction(tr("Hide Menu"));
	hideMenu2->setCheckable(true);
	hideMenu2->setChecked(settwin->isMenuHidden());
	QObject::connect(hideMenu2, SIGNAL(triggered(bool)), this, SLOT(hidemenu(bool)));

	QAction* setting = contextmenu->addAction(tr("Settings"));
	QObject::connect(setting, SIGNAL(triggered(bool)), settwin, SLOT(show()));
}
