#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include "settings.h"
#include <QWidget>
#include <QGridLayout>
#include <QTextBrowser>
#include <QPushButton>

class HelpWindow : public QWidget
{
	Q_OBJECT

	QGridLayout grid;
	QTextBrowser browser;
	QPushButton closeButton;

public:
	HelpWindow(QWidget* parent = nullptr);
	virtual ~HelpWindow() override;

	void setLanguage(Settings::Language lang);

public slots:
	void retranslate();
};

#endif // HELPWINDOW_H
