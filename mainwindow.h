#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicalwindow.h"
#include "settingswindow.h"
#include "separatethread.h"
#include <QMainWindow>

class QCloseEvent;
class QGridLayout;
class QMenuBar;
class QMenu;
class QFile;
class QTranslator;

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

	static QTranslator translate;
	static QTranslator qtranslate;

public:
	explicit MainWindow(QWidget* parent = nullptr);
	MainWindow(const QString& file, QWidget* parent = nullptr);
	virtual ~MainWindow() override;

	static void changeLanguage(int lang);


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
	void showContextMenu(const QPoint& pos);
	void saveGame();
	void loadGame();
	void readSavefile(QFile& file);
	void readStartfile(QFile& file);

signals:
	void startThread();
};

#endif // MAINWINDOW_H
