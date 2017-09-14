#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicalwindow.h"
#include "settingswindow.h"
#include "separatethread.h"
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QCloseEvent>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QWidget* mainwidget;

	GraphicalWindow* graphic;
	SettingsWindow* settwin;
	QGraphicsScene* scene;

	Label* generationTimer;

	QGridLayout* grid;

	void createLayout();

	QMenuBar* menubar;
	QMenu* start;
		QAction* startnew;
		QAction* open;
		QAction* save;
		QAction* clearall;
		QAction* autoplays;
		QAction* pauses;
		QAction* resumes;
	QMenu* view;
		QAction* hideMenu;
		QAction* maximize;
		QAction* fullscreen;
	QMenu* settings;
		QAction* setting;
	QMenu* help;
		QAction* help1;

	void createMenu();
	void createSubMenu();

	QMenu* contextmenu;
		QAction* hideMenu2;

	void createContextMenu();

	void closeEvent(QCloseEvent* event);

	void customShow();

	int autoplayBreak;

	QThread* secondThread;
	SeparateThread* worker;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void startNew();
	void clearAll();
	void helpWindow();
	void about();
	void autoplay();
	void pause();
	void resume();
	void updates();
	void hidemenu(bool activated);
	void showmenu();
	void showContextMenu(const QPoint &pos);
	void saveGame();
	void openGame();

signals:
	void startThread();
};

#endif // MAINWINDOW_H
