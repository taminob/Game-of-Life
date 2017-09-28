#include "helpwindow.h"
#include "mainwindow.h"

HelpWindow::HelpWindow(QWidget *parent) : QWidget(parent), closeButton(tr("&Close"))
{
	this->setWindowTitle(tr("Help"));
	this->setWindowIcon(MainWindow::iconStyle.standardIcon(QStyle::SP_DialogHelpButton));

	browser.setMinimumSize(400, 500);
	browser.setSearchPaths(QStringList(":/"));
	browser.setSource(QUrl("en_help.html"));

	grid.addWidget(&browser, 0, 0, 1, 2);
	grid.addWidget(&closeButton, 1, 1);

	this->setLayout(&grid);
}

HelpWindow::~HelpWindow()
{

}

void HelpWindow::setLanguage(Settings::Language lang)
{
	if(lang == Settings::Language::English)
		browser.setSource(QUrl("en_help.html"));
	else if(lang == Settings::Language::German)
		browser.setSource(QUrl("de_help.html"));

	retranslate();
}

void HelpWindow::retranslate()
{
	this->setWindowTitle(tr("Help"));
	closeButton.setText(tr("&Close"));
}
