#include "label.h"
#include <QApplication>

Label::Label(Settings* settings, std::size_t value, QWidget* parent) : QLabel(parent)
{
	this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	this->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	this->setWindowFlags(Qt::SubWindow);
	this->setFont(QFont("", settings->getTimerSize()));
	this->setForegroundRole(QPalette::BrightText);

	this->value = value;
	this->settings = settings;
	this->setNum(static_cast<int>(value));
}

void Label::changeTimer(const unsigned int& newValue)
{
	value = newValue;
	this->setNum(static_cast<int>(value));
}

void Label::newTimer()
{
	this->setFont(QFont("", settings->getTimerSize()));
	this->setForegroundRole(QPalette::BrightText);

	this->setNum(0);
}
