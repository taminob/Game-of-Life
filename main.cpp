#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	if(QLocale::system().language() == QLocale::German)
		MainWindow::changeLanguage(1);
	else
		MainWindow::changeLanguage(0);

	MainWindow win(argv[argc - 1]);

	return app.exec();
}
