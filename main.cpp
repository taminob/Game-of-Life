// © Copyright (c) 2018 SqYtCO

#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/images/gol-icon-90.png"));
	app.setApplicationDisplayName("Game Of Life");

	// set application style
	app.setStyle(QStyleFactory::create("Fusion"));

	// set all colors
	QPalette dark_palette;
	dark_palette.setColor(QPalette::Window, QColor(0x30, 0x30, 0x30));
	dark_palette.setColor(QPalette::WindowText, Qt::white);
	dark_palette.setColor(QPalette::Base, QColor(0x10, 0x10, 0x10));
	dark_palette.setColor(QPalette::AlternateBase, QColor(0xF0, 0xF0, 0xF0));
	dark_palette.setColor(QPalette::ToolTipBase, Qt::black);
	dark_palette.setColor(QPalette::ToolTipText, Qt::white);
	dark_palette.setColor(QPalette::Text, Qt::white);
	dark_palette.setColor(QPalette::Button, QColor(0x10, 0x10, 0x10));
	dark_palette.setColor(QPalette::ButtonText, Qt::white);
	dark_palette.setColor(QPalette::BrightText, Qt::white);
	dark_palette.setColor(QPalette::Link, QColor(0x00, 0x10, 0xFF));
	dark_palette.setColor(QPalette::Highlight, QColor(0xF0, 0xF0, 0xF0));
	dark_palette.setColor(QPalette::HighlightedText, Qt::black);

	// set default palette in application
	app.setPalette(dark_palette);

	// if more than 1 argument is given, pass last argument as start_file
	MainWindow win((argc > 1) ? argv[argc - 1] : nullptr);
	win.show();

	return app.exec();
}
