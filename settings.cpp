#include "settings.h"
#include "settingswindow.h"
#include "separatethread.h"
#include "graphicalwindow.h"
#include <QColorDialog>
#include <QSettings>
#include <QPalette>
#include <QVector>

Settings::Settings(SettingsWindow* settwin, QObject* parent) : QObject(parent), settwin(settwin), settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, "Game_Of_Life", "Game_Of_Life", this))
{
	if(settings->status() == QSettings::NoError && settings->contains("General/windowStartmodus"))
	{
		loadSettings();
	}
	else
	{
		setDefaultValues();
		gameNumber = 0;
	}

	saved = true;
}

void Settings::saveSettings()
{
	if(!saved && settings->isWritable())
	{
		settings->beginGroup("General");
		settings->setValue("windowStartmodus", windowStartmodus);
		settings->setValue("fixedSize", fixedSize);
		settings->setValue("fixedWidth", fixedWidth);
		settings->setValue("fixedHeight", fixedHeight);
		settings->setValue("hiddenMenu", hiddenMenu);
		settings->setValue("gameNumber", gameNumber);
		settings->setValue("aliveCellColor", aliveCellColor.name());
		settings->setValue("deadCellColor", deadCellColor.name());
		settings->setValue("lineColor", lineColor.name());
		settings->setValue("nextGenAliveCellColor", nextGenAliveCellColor.name());
		settings->setValue("nextGenDeadCellColor", nextGenDeadCellColor.name());
		settings->setValue("backgroundColor", backgroundColor.name());
		settings->setValue("exitWarning", exitWarning);
		settings->setValue("dynSettingChange", dynSettingChange);
		settings->setValue("customColorsSize", customColorsSize);
		for(int i = 0; i < customColors.size(); ++i)
			settings->setValue("customColors" + QString::number(i), customColors[i].name());
		settings->setValue("scrollBarEnabled", scrollBarEnabled);
		settings->endGroup();
		settings->beginGroup("Game");
		settings->setValue("autoplaySpeed", autoplaySpeed);
		settings->setValue("maxAutoplayEnabled", maxAutoplayEnabled);
		settings->setValue("maxAutoplay", maxAutoplay);
		settings->setValue("cellSize", cellSize);
		settings->setValue("gridWidth", gridWidth);
		settings->setValue("x", x);
		settings->setValue("y", y);
		settings->setValue("timerSize", timerSize);
		settings->endGroup();
	}

	saved = true;
}

void Settings::loadSettings()
{
	windowStartmodus = settings->value("General/windowStartmodus").toUInt();
	fixedSize = settings->value("General/fixedSize").toBool();
	fixedHeight = settings->value("General/fixedHeight").toUInt();
	fixedWidth = settings->value("General/fixedWidth").toUInt();
	hiddenMenu = settings->value("General/hiddenMenu").toBool();
	gameNumber = settings->value("General/gameNumber").toUInt();
	aliveCellColor.setNamedColor(settings->value("General/aliveCellColor").toString());
	deadCellColor.setNamedColor(settings->value("General/deadCellColor").toString());
	lineColor.setNamedColor(settings->value("General/lineColor").toString());
	nextGenAliveCellColor.setNamedColor(settings->value("General/nextGenAliveCellColor").toString());
	nextGenDeadCellColor.setNamedColor(settings->value("General/nextGenDeadCellColor").toString());
	backgroundColor.setNamedColor(settings->value("General/backgroundColor").toString());
	exitWarning = settings->value("General/exitWarning").toBool();
	dynSettingChange = settings->value("General/dynSettingChange").toBool();
	customColorsSize = settings->value("General/customColorsSize").toUInt();
	customColors.resize(customColorsSize);
	for(std::size_t i = 0; i < customColorsSize; ++i)
	{
		customColors[i].setNamedColor(settings->value("General/customColors" + QString::number(i)).toString());
	}
	scrollBarEnabled = settings->value("General/scrollBarEnabled").toInt();

	autoplaySpeed = settings->value("Game/autoplaySpeed").toUInt();
	maxAutoplayEnabled = settings->value("Game/maxAutoplayEnabled").toBool();
	maxAutoplay = settings->value("Game/maxAutoplay").toInt();
	cellSize = settings->value("Game/cellSize").toUInt();
	gridWidth = settings->value("Game/gridWidth").toUInt();
	x = settings->value("Game/x").toUInt();
	y = settings->value("Game/y").toUInt();
	timerSize = settings->value("Game/timerSize").toUInt();
}

void Settings::setDefaultValues()
{
	// default values
	autoplaySpeed = 500;
	maxAutoplayEnabled = false;
	maxAutoplay = 100;
	fixedHeight = 700;
	fixedWidth = 1000;
	windowStartmodus = 0;
	fixedSize = false;
	exitWarning = true;
	dynSettingChange = true;
	gridWidth = 1;
	cellSize = 40;
	x = 10;
	y = 10;
	hiddenMenu = false;
	timerSize = 40;
	deadCellColor = Qt::black;
	aliveCellColor = Qt::green;
	lineColor = Qt::white;
	nextGenAliveCellColor.setRgb(0x22, 0x22, 0x22);
	nextGenDeadCellColor.setRgb(0x66, 0xFF, 0x66);
	backgroundColor = Qt::black;
	customColors.resize(QColorDialog::customCount());
	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = Qt::white;
	customColorsSize = QColorDialog::customCount();
	scrollBarEnabled = 0;
}

void Settings::setNewAutoplaySpeed(int newValue)
{
	saved = false;

	autoplaySpeed = newValue;

	emit autoplaySpeedChanged();
}

void Settings::maxAutoplayActivated(bool activated)
{
	saved = false;

	maxAutoplayEnabled = activated;

	if(activated)
	{
		settwin->maxAutoplaySpinbox->setDisabled(false);
	}
	else
	{
		settwin->maxAutoplaySpinbox->setDisabled(true);
	}
}

void Settings::setNewMaxAutoplay(int newValue)
{
	saved = false;

	maxAutoplay = newValue;
}

void Settings::setNewWindowStartmodus(int newValue)
{
	/*
	 * 0 - Standard
	 * 1 - Maximized
	 * 2 - Minimized
	 * 3 - Fullscreen
	 * 4 - Custom Fixed Size
	 */

	saved = false;

	windowStartmodus = newValue;
}

void Settings::fixedSizeActivated(bool activated)
{
	saved = false;

	if(activated)
	{
		settwin->windowStartmodusCombobox->setDisabled(true);
		settwin->fixedSizeWidthEnter->setDisabled(false);
		settwin->fixedSizeHeightEnter->setDisabled(false);

		fixedSize = true;
	}
	else
	{
		settwin->windowStartmodusCombobox->setDisabled(false);
		settwin->fixedSizeWidthEnter->setDisabled(true);
		settwin->fixedSizeHeightEnter->setDisabled(true);

		fixedSize = false;
	}
}

void Settings::setNewFixedWidth(int newValue)
{
	saved = false;

	fixedWidth = newValue;
}

void Settings::setNewFixedHeight(int newValue)
{
	saved = false;

	fixedHeight = newValue;
}

void Settings::hideMenuActivated(bool activated)
{
	saved = false;

	hiddenMenu = activated;
}

void Settings::exitWarningActivated(bool activated)
{
	saved = false;

	exitWarning = activated;
}

void Settings::setNewCellSize(int newValue)
{
	saved = false;

	cellSize = newValue;
}

void Settings::setNewGridWidth(int newValue)
{
	saved = false;

	gridWidth = newValue;
}

void Settings::setNewX(int newValue)
{
	saved = false;

	x = newValue;
}

void Settings::setNewY(int newValue)
{
	saved = false;

	y = newValue;
}

void Settings::setNewTimerSize(int newValue)
{
	saved = false;

	timerSize = newValue;
}

void Settings::setGameNumber(int newValue)
{
	saved = false;

	gameNumber = newValue;
}

void Settings::newAliveCellColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	aliveCellColor = QColorDialog::getColor(aliveCellColor, settwin, tr("Alive Cell Color"));

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	aliveCellColorPalette->setColor(QPalette::Window, aliveCellColor);
	settwin->aliveCellColorPreview->setPalette(*aliveCellColorPalette);

	if(dynSettingChange)
		settwin->graphic->scene->update(settwin->graphic->sceneRect());
}

void Settings::newDeadCellColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	deadCellColor = QColorDialog::getColor(deadCellColor, settwin, tr("Dead Cell Color"));

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	deadCellColorPalette->setColor(QPalette::Window, deadCellColor);
	settwin->deadCellColorPreview->setPalette(*deadCellColorPalette);

	if(dynSettingChange)
		settwin->graphic->scene->update(settwin->graphic->sceneRect());
}

void Settings::newLineColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	lineColor = QColorDialog::getColor(lineColor, settwin, tr("Line Color"));

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	lineColorPalette->setColor(QPalette::Window, lineColor);
	settwin->lineColorPreview->setPalette(*lineColorPalette);

	if(dynSettingChange)
		settwin->graphic->scene->update(settwin->graphic->sceneRect());
}

void Settings::newNextGenAliveCellColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	nextGenAliveCellColor = QColorDialog::getColor(nextGenAliveCellColor, settwin, tr("Next generation born cell color"));

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	nextGenAliveCellColorPalette->setColor(QPalette::Window, nextGenAliveCellColor);
	settwin->nextGenAliveCellColorPreview->setPalette(*nextGenAliveCellColorPalette);

	if(dynSettingChange)
		settwin->graphic->scene->update(settwin->graphic->sceneRect());
}

void Settings::newNextGenDeadCellColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	nextGenDeadCellColor = QColorDialog::getColor(nextGenDeadCellColor, settwin, tr("Next generation born cell color"));

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	nextGenDeadCellColorPalette->setColor(QPalette::Window, nextGenDeadCellColor);
	settwin->nextGenDeadCellColorPreview->setPalette(*nextGenDeadCellColorPalette);

	if(dynSettingChange)
		settwin->graphic->scene->update(settwin->graphic->sceneRect());
}

void Settings::newBackgroundColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	backgroundColor = QColorDialog::getColor(backgroundColor, settwin, tr("Background Color"));

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	backgroundColorPalette->setColor(QPalette::Window, backgroundColor);
	settwin->backgroundColorPreview->setPalette(*backgroundColorPalette);

	if(dynSettingChange)
		settwin->graphic->scene->update(settwin->graphic->sceneRect());
}

void Settings::dynSettingChangeEnabled(bool activated)
{
	saved = false;

	dynSettingChange = activated;
}

void Settings::newScrollBarPolicy(int newValue)
{
	saved = false;

	// 0 = Always Off
	// 1 = Always On
	// 2 = As Needed

	scrollBarEnabled = newValue;

	if(dynSettingChange && scrollBarEnabled == 0)
	{
		settwin->graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		settwin->graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else if(dynSettingChange && scrollBarEnabled == 1)
	{
		settwin->graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		settwin->graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else if(dynSettingChange && scrollBarEnabled == 2)
	{
		settwin->graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		settwin->graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}
