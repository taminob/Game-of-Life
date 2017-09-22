#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <QCloseEvent>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>
#include <QTextStream>
#include <QDir>

MainWindow::MainWindow(QWidget* parent, Language language) : QMainWindow(parent)
{
	installLanguage(language);

	mainwidget = new QWidget;
	settwin = new SettingsWindow;

	QObject::connect(settwin->settings(), &Settings::languageChanged, this, &MainWindow::changeLanguage);
	changeLanguage(settwin->settings()->getLanguage());

	qApp->setWindowIcon(QIcon(":/images/game-of-life_icon.jpg"));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	createContextMenu();
	QObject::connect(this, &MainWindow::customContextMenuRequested, this, &MainWindow::showContextMenu);

	grid = new QGridLayout;

	graphic = new GraphicalWindow(settwin->settings());
	graphic->setContextMenu(contextmenu);
	scene = new QGraphicsScene(this);
	graphic->setCellScene(scene, settwin->settings()->getX(), settwin->settings()->getY());
	generationTimer = new Label(settwin->settings(), graphic->system.getGeneration(), this);
	graphic->stackUnder(generationTimer);
	grid->addWidget(graphic, 0, 0, 2, 2);
	grid->addWidget(generationTimer, 1, 1);
	mainwidget->setLayout(grid);
	this->setCentralWidget(mainwidget);

	createMenu();

	customShow();

	if(settwin->settings()->isMenuHidden())
	{
		this->hidemenu(true);
	}
	else
	{
		showmenu();
	}

	secondThread = new QThread(this);
	autoplayBreak = -3;
	worker = new SeparateThread(graphic, settwin->settings(), generationTimer, graphic->system, autoplayBreak);
	worker->moveToThread(secondThread);
	QObject::connect(this, &MainWindow::startThread, worker, &SeparateThread::work);
	secondThread->start();

	settwin->setGraphicalWidgets(graphic, generationTimer);

	exitBox = new QMessageBox(QMessageBox::Question, tr("Exit"), tr("Do you really want to exit?"), QMessageBox::Yes | QMessageBox::No, this);
}

MainWindow::MainWindow(const QString& file, Language language, QWidget* parent) : MainWindow(parent, language)
{
	int pos = file.lastIndexOf('.');
	if(file.midRef(pos == -1 ? 0 : pos) == ".save")
	{
		QFile f(file);
		readSavefile(f);
	}
	else if(file.midRef(pos == -1 ? 0 : pos) == ".start")
	{
		QFile f(file);
		readStartfile(f);
	}
}

MainWindow::~MainWindow()
{
	pause();
	secondThread->wait(1000);
	secondThread->quit();

	delete settwin;
	delete worker;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if(settwin->settings()->isExitWarningEnabled())
	{
	//	QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Exit"), tr("Do you really want to exit?"), QMessageBox::Yes | QMessageBox::No);
		QMessageBox::StandardButton btn = static_cast<QMessageBox::StandardButton>(exitBox->exec());

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
	menubar = new QMenuBar(this);

	start = menubar->addMenu(tr("&Start"));
	createStartSubMenu();
	view = menubar->addMenu(tr("&View"));
	createViewSubMenu();
	settings = menubar->addMenu(tr("Se&ttings"));
	createSettingsSubMenu();
	help = menubar->addMenu(tr("&Help"));
	createHelpSubMenu();

	this->setMenuBar(menubar);
}

void MainWindow::createStartSubMenu()
{
	startnew = start->addAction(tr("&New"));
	startnew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
	QObject::connect(startnew, &QAction::triggered, this, &MainWindow::startNew);
	this->addAction(startnew);
	open = start->addAction(tr("&Open"));
	open->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	QObject::connect(open, &QAction::triggered, this, &MainWindow::loadGame);
	this->addAction(open);
	save = start->addAction(tr("&Save"));
	save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	QObject::connect(save, &QAction::triggered, this, &MainWindow::saveGame);
	this->addAction(save);
	clearall = start->addAction(tr("&Clear"));
	clearall->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	QObject::connect(clearall, &QAction::triggered, this, &MainWindow::clearAll);
	this->addAction(clearall);
	autoplays = start->addAction(tr("&Auto Play"));
	autoplays->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	QObject::connect(autoplays, &QAction::triggered, this, &MainWindow::autoplay);
	this->addAction(autoplays);
	resumes = start->addAction(tr("&Resume"));
	resumes->setShortcut(QKeySequence(Qt::Key_Space));
	QObject::connect(resumes, &QAction::triggered, this, &MainWindow::resume);
	this->addAction(resumes);
	pauses = start->addAction(tr("&Pause"));
	pauses->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	QObject::connect(pauses, &QAction::triggered, this, &MainWindow::pause);
	this->addAction(pauses);
//	QAction* updateScene = start->addAction(tr("&Update"));								// Not anymore required
//	QObject::connect(updateScene, &QAction::triggered, this, &MainWindow::updates);		// Not anymore required

	start->addSeparator();
	QAction* startExit = start->addAction(tr("E&xit"));
//	startExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));					// old shortcut
	startExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	QObject::connect(startExit, &QAction::triggered, this, &MainWindow::close);
	this->addAction(startExit);
}

void MainWindow::createViewSubMenu()
{
	hideMenu = view->addAction(tr("&Hide Menu"));
	hideMenu->setCheckable(true);
	hideMenu->setChecked(settwin->settings()->isMenuHidden());
	hideMenu->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
	QObject::connect(hideMenu, &QAction::triggered, this, &MainWindow::hidemenu);
	this->addAction(hideMenu);
	view->addSeparator();
	QAction* minimize = view->addAction(tr("M&inimize"));
	QObject::connect(minimize, &QAction::triggered, this, &MainWindow::showMinimized);
	maximize = view->addAction(tr("&Maximize"));
	maximize->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	QObject::connect(maximize, &QAction::triggered, this, &MainWindow::showMaximized);
	this->addAction(maximize);
	fullscreen = view->addAction(tr("&Fullscreen"));
	fullscreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
	QObject::connect(fullscreen, &QAction::triggered, this, &MainWindow::showFullScreen);
	this->addAction(fullscreen);
}

void MainWindow::createSettingsSubMenu()
{
	if(currentLanguage == Language::English)
		languages = settings->addMenu(QIcon(":/images/gb_flag.png"), tr("Language"));
	else if(currentLanguage == Language::German)
		languages = settings->addMenu(QIcon(":/images/de_flag.png"), tr("Language"));
	else
		languages = settings->addMenu(tr("Language"));
	QAction* english = languages->addAction(QIcon(":/images/gb_flag.png"), tr("English"));
	english->setCheckable(true);
	english->setChecked(currentLanguage == Language::English);
	QObject::connect(english, &QAction::triggered, [this]() { this->changeLanguage(Language::English);} );
	QAction* german = languages->addAction(QIcon(":/images/de_flag.png"), tr("German"));
	german->setCheckable(true);
	german->setChecked(currentLanguage == Language::German);
	QObject::connect(german, &QAction::triggered, [this]() { this->changeLanguage(Language::German);} );

	QAction* generalSetting = settings->addAction(tr("&General"));
	QObject::connect(generalSetting, &QAction::triggered, settwin, &SettingsWindow::showGeneral);
	QAction* gameSetting = settings->addAction(tr("G&ame"));
	QObject::connect(gameSetting, &QAction::triggered, settwin, &SettingsWindow::showGame);
	settings->addSeparator();
	setting = settings->addAction(QIcon(":/images/preferences-system.png"), tr("&Settings"));
	setting->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	QObject::connect(setting, &QAction::triggered, settwin, &MainWindow::show);
	this->addAction(setting);
}

void MainWindow::createHelpSubMenu()
{
	help1 = help->addAction(iconStyle.standardIcon(QStyle::SP_DialogHelpButton), tr("&Help"));
	help1->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
	QObject::connect(help1, &QAction::triggered, this, &MainWindow::helpWindow);
	this->addAction(help1);
	help->addSeparator();
	QAction* aboutQt = help->addAction(QIcon(":/images/qt_icon.png"), tr("About &Qt"));
	QObject::connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
	QAction* aboutAction = help->addAction(QIcon(":images/game-of-life_icon.jpg"), tr("&About"));
	QObject::connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::startNew()
{
	pause();

	graphic->setCellScene(scene, settwin->settings()->getX(), settwin->settings()->getY());
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
	autoplayBreak = settwin->settings()->getMaxAutoplay();
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

void MainWindow::updates()
{
	graphic->fullUpdate();
	settwin->settings()->loadSettings();
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

	if(settwin->settings()->isFixed())
	{
		this->setFixedSize(QSize(settwin->settings()->getFixedWidth(), settwin->settings()->getFixedHeight()));
		this->show();
	}
	else
	{
		if(settwin->settings()->getWindowStartmodus() == 0)
		{
			this->show();
		}
		else if(settwin->settings()->getWindowStartmodus() == 1)
		{
			this->showMaximized();
		}
		else if(settwin->settings()->getWindowStartmodus() == 2)
		{
			this->showMinimized();
		}
		else if(settwin->settings()->getWindowStartmodus() == 3)
		{
			this->showFullScreen();
		}
	}
}

void MainWindow::helpWindow()
{
	QMessageBox::information(this, tr("Help"), tr("Here you can get help!"));
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About"), tr("<b>Sources:</b> <br> https://www.github.com/sqytco/game-of-life <br><br> <b>Contact:</b> <br> sqyt.co@gmail.com"));
}

void MainWindow::showContextMenu(const QPoint& pos)
{
	contextmenu->exec(mapToGlobal(pos));
}

void MainWindow::createContextMenu()
{
	contextmenu = new QMenu(tr("Context Menu"), this);

	hideMenu2 = contextmenu->addAction(tr("Hide Menu"));
	hideMenu2->setCheckable(true);
	hideMenu2->setChecked(settwin->settings()->isMenuHidden());
	QObject::connect(hideMenu2, &QAction::triggered, this, &MainWindow::hidemenu);

	QAction* setting = contextmenu->addAction(tr("Settings"));
	QObject::connect(setting, &QAction::triggered, settwin, &SettingsWindow::show);
}

void MainWindow::saveGame()
{
	QString filename = QString::number(settwin->settings()->getGameNumber());
	filename += "_SAVEFILE-GAME_OF_LIFE.save";

	if(!QDir("save").exists())
		QDir().mkdir("save");

	while(QFile("save/" + filename).exists())
	{
		std::size_t temp = settwin->settings()->getGameNumber() + 1;

		QMessageBox::StandardButtons btn = QMessageBox::warning(this, tr("Overwrite existing savefile?"), filename + tr(" exists already!\nOverwrite?"), QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No);

		if(btn == QMessageBox::Yes)
		{
			break;
		}
		else if(btn == QMessageBox::No)
		{
			filename.replace(QString::number(settwin->settings()->getGameNumber()), QString::number(temp));
			settwin->settings()->setGameNumber(settwin->settings()->getGameNumber() + 1);
		}
		else
		{
			return;
		}
	}
	filename.insert(0, "save/");

	QFile file(filename);
	if(file.open(QIODevice::Truncate | QIODevice::WriteOnly))
	{
		QDataStream stream(&file);

		SeparateThread::systemMutex.lock();
		stream << static_cast<int>(graphic->system.getGeneration())
			   << static_cast<int>(graphic->system.size()) - 1
			   << static_cast<int>(graphic->system[0].size()) - 1;

		for(std::size_t a = 0; a < graphic->system.size(); ++a)
		{
			for(std::size_t b = 0; b < graphic->system[0].size(); ++b)
			{
				stream << static_cast<int>(graphic->system[a][b].alive);
			}
		}
		SeparateThread::systemMutex.unlock();
	}
	else
		QMessageBox::warning(this, tr("ERROR"), tr("Game can not be saved!"), QMessageBox::Ok);

	settwin->settings()->setting()->beginGroup("General");
	settwin->settings()->setGameNumber(settwin->settings()->getGameNumber() + 1);
	settwin->settings()->setting()->setValue("gameNumber", settwin->settings()->getGameNumber());
	settwin->settings()->setting()->endGroup();

	settwin->updateSettings();
}

void MainWindow::loadGame()
{
	QString selectedFilter(tr("Saved Games (*.save)"));
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Savefile..."), QDir::currentPath() + "/save", tr("All Files (*.*);; Saved Games (*.save);; Startfiles (*.start)"), &selectedFilter);
	if(filename.size() > 5)
	{
		int pos = filename.lastIndexOf('.');
		if(filename.midRef(pos == -1 ? 0 : pos) == ".save")
		{
			QFile f(filename);
			readSavefile(f);
		}
		else if(filename.midRef(pos == -1 ? 0 : pos) == ".start")
		{
			QFile f(filename);
			readStartfile(f);
		}
		else
		{
			QMessageBox::information(this, tr("Invalid File"), filename + tr(" can not be opened!"));
		}
	}
	else if(filename.size() == 0)
	{

	}
	else
	{
		QMessageBox::information(this, tr("Invalid File"), filename + tr(" is invalid!"));
	}
}

void MainWindow::readSavefile(QFile& file)
{
	if(file.open(QIODevice::ReadOnly))
	{
		int generation, w, h;
		int alive;

		QDataStream stream(&file);

		stream >> generation >> w >> h;
		std::vector<Cell> row;
		SeparateThread::systemMutex.lock();
		graphic->system.clear();

		for(int a = 0; a <= w; ++a)
		{
			for(int b = 0; b <= h; ++b)
			{
				stream >> alive;

				if(alive)
					row.push_back(Cell(true));
				else
					row.push_back(Cell(false));
			}

			graphic->system.push_back(std::move(row));
			row.clear();
		}

		graphic->system.setGeneration(generation);
		graphic->system.calcNextGen();
		SeparateThread::systemMutex.unlock();
		graphic->setCellScene(graphic->scene, w + 1, h + 1, false);
		generationTimer->changeTimer(generation);
	}
	else
	{
		QMessageBox::information(this, tr("Reading Error"), tr("File can not be opened!"), QMessageBox::Cancel);
	}
}

void MainWindow::readStartfile(QFile& file)
{
	if(file.open(QIODevice::ReadOnly))
	{
		int w = 0, h = 0;
		char alive;

		QTextStream stream(&file);

		std::vector<Cell> row;
		SeparateThread::systemMutex.lock();
		graphic->system.clear();
		SeparateThread::systemMutex.unlock();

		std::vector<bool> cellStates;

		while(stream.status() == QTextStream::Ok)
		{
			stream >> alive;

			if(alive == '1')
			{
				if(h == 0)
					++w;
				cellStates.push_back(true);
			}
			else if(alive == '0')
			{
				if(h == 0)
					++w;
				cellStates.push_back(false);
			}
			else if(alive == '\n')
				++h;
		}

		if(w < 2)
			for(int i = 0; i < h; ++i)
				cellStates.push_back(false);
		else if(h < 2)
			for(int i = 0; i < w; ++i)
				cellStates.push_back(false);

		while(static_cast<std::size_t>(w * h) > cellStates.size())	// static_cast: avoid sign-compare warning
			cellStates.push_back(false);

		SeparateThread::systemMutex.lock();
		for(int a = 0; a < w; ++a)
		{
			for(int b = 0; b < h; ++b)
			{
				row.push_back(Cell(cellStates[b + a * h]));
			}

			graphic->system.push_back(std::move(row));
			row.clear();
		}

		graphic->system.setGeneration(0);
		graphic->system.calcNextGen();
		SeparateThread::systemMutex.unlock();
		graphic->setCellScene(graphic->scene, w, h, false);
		generationTimer->changeTimer(0);
	}
	else
	{
		QMessageBox::information(this, tr("Reading Error"), tr("File can not be opened!"), QMessageBox::Cancel);
	}
}

void MainWindow::changeLanguage(Language lang)
{
	if(lang != currentLanguage)
	{
		currentLanguage = lang;
		settwin->settings()->setLanguage(lang);
		settwin->settings()->saveSettings();

		installLanguage(lang);
	}
}
#include <QLibraryInfo>
void MainWindow::installLanguage(Language lang)
{
//	qApp->removeTranslator(&translate);			// not necessary
//	qApp->removeTranslator(&qtranslate);		// not necessary

	if(lang == Language::English)
	{
		if(!translate.load(":/translations/gol_en"))
			qDebug("No EN-translation found!");
		if(!qtranslate.load(":/translations/qt_en"))
			qDebug("No EN-Qt-translation found!");
	}
	else if(lang == Language::German)
	{
		if(!translate.load(":/translations/gol_de"))
			qDebug("No DE-translation found!");
		if(!qtranslate.load("qt_de", ":/translations"))
	//	if(!qtranslate.load("qt_de", QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
			qDebug("No DE-Qt-translation found!");
	}

	qApp->installTranslator(&translate);
	qApp->installTranslator(&qtranslate);

	qApp->exit(197 + static_cast<int>(lang));
}
