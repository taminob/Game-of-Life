// © Copyright (c) 2018 SqYtCO

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/images/gol-icon-90.png"));

	// if more than 1 argument is given, pass last argument as start_file
	MainWindow win((argc > 1) ? argv[argc - 1] : nullptr);
	win.show();

	return app.exec();
}
