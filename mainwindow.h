#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicalwindow.h"
#include "settingswindow.h"
#include "separatethread.h"
#include <QMainWindow>
#include <QTranslator>
#include <QCommonStyle>

class QMessageBox;
class QCloseEvent;
class QGridLayout;
class QMenuBar;
class QMenu;
class QFile;

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
		QMenu* languages;
		QAction* setting;
	QMenu* help;
		QAction* help1;

	QCommonStyle iconStyle;

	void createMenu();
	void createStartSubMenu();
	void createViewSubMenu();
	void createSettingsSubMenu();
	void createHelpSubMenu();

	QMenu* contextmenu;
		QAction* hideMenu2;

	void createContextMenu();

	QMessageBox* exitBox;
	void closeEvent(QCloseEvent* event);

	void customShow();

	int autoplayBreak;

	QThread* secondThread;
	SeparateThread* worker;

	QTranslator translate;
	QTranslator qtranslate;

public:
	using Language = Settings::Language;

	void changeLanguage(Language lang);

private:
	void installLanguage(Language lang);
	Language currentLanguage;

public:
	explicit MainWindow(QWidget* parent = nullptr, Language language = Language::English);
	MainWindow(const QString& file, Language language = Language::English, QWidget* parent = nullptr);
	virtual ~MainWindow() override;

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
