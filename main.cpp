#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QTranslator translate;
	if(QLocale::system().language() == QLocale::German)
	{
		if(!translate.load(":/translations/gol_de"))
			qDebug("No DE-translation found!");
	}
	else
	{
		if(!translate.load(":/translations/gol_en"))
			qDebug("No EN-translation found!");

	}
	app.installTranslator(&translate);

	QTranslator qtranslate;
	if(QLocale::system().language() == QLocale::German)
	{
		if(!qtranslate.load(":/qt_de"))
			qDebug("No Qt-Translation found!");
	}
	else
	{
		if(!qtranslate.load(":/qt_en"))
			qDebug("No Qt-Translation found!");
	}
	app.installTranslator(&qtranslate);

	MainWindow win(argv[argc - 1]);

	return app.exec();
}
