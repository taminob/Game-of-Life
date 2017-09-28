#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	MainWindow win(argv[argc - 1]);

	return app.exec();
}
