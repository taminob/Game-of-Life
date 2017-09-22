#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	int ret = 197;

	while(true)
	{
		MainWindow win(argv[argc - 1], static_cast<MainWindow::Language>(ret - 197));

		 ret = app.exec();

		if(ret < 197 || ret > 198)			// ret = 197: English (0); ret = 198: German (1)
			return ret;
	}
}
