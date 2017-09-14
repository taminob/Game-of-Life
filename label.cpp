#include "label.h"
#include <QApplication>

Label::Label(SettingsWindow* settings, unsigned int value, QWidget* parent) : QLabel(parent)
{
	this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	this->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	this->setWindowFlags(Qt::SubWindow);
	this->setFont(QFont("", settings->getTimerSize()));
	this->setForegroundRole(QPalette::BrightText);
//	this->setForegroundRole(settings->getTimerColor());		// new feature

	this->value = value;
	this->settings = settings;
	this->setNum(static_cast<int>(value));
}

void Label::changeTimer(unsigned int newValue)
{
	value = newValue;
	this->setNum(static_cast<int>(value));
}

void Label::newTimer()
{
	this->setFont(QFont("", settings->getTimerSize()));
	this->setForegroundRole(QPalette::BrightText);
//	this->setForegroundRole(settings->getTimerColor());		// new feature

	this->setNum(0);
}
