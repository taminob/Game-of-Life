#include "settingswindow.h"
#include "graphicalwindow.h"
#include "label.h"
#include <QAction>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QSignalMapper>
#include <QLabel>
#include <QColorDialog>
#include <QFileDialog>
#include <QDataStream>
#include <QDir>

#include <climits>

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent)
{
	graphic = 0;

	this->setWindowTitle(tr("Settings"));
	this->setWindowModality(Qt::ApplicationModal);			// blocks the other windows
	this->setWindowFlags(Qt::WindowCloseButtonHint);		// shows only close button

	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Game_Of_Life", "Game_Of_Life");
	if(settings->status() == QSettings::NoError && settings->contains("General/windowStartmodus"))
	{
		loadSettings();
	}
	else
	{
		setDefaultValues();
		gameNumber = 0;
	}

	tabs = new QTabWidget;
	createGeneralTab();
	tabs->addTab(generaltab, tr("General"));
	createGameTab();
	tabs->addTab(gametab, tr("Game"));

	QDialogButtonBox* diabutts = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close);
	QObject::connect(diabutts->button(QDialogButtonBox::Save), SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
	QObject::connect(diabutts->button(QDialogButtonBox::Close), SIGNAL(clicked(bool)), this, SLOT(close()));

	createShortcuts();

	grid = new QGridLayout;

	grid->addWidget(tabs, 0, 0, 1, 5);
	grid->addWidget(diabutts, 3, 3);

	this->setLayout(grid);

	saved = true;
}

SettingsWindow::~SettingsWindow()
{
	delete aliveCellColorPalette;
	delete deadCellColorPalette;
	delete settings;
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
	if(!saved)
	{
		QMessageBox::Button btn = QMessageBox::question(this, tr("Save?"), tr("Do you want to save changes?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

		if(btn == QMessageBox::Yes)
		{
			saveSettings();
			saved = true;
			event->accept();
		}
		else if(btn == QMessageBox::No)
		{
			if(settings->status() == QSettings::NoError && settings->contains("General/windowStartmodus"))
				loadSettings();
			else
				setDefaultValues();
			updateSettings();
			saved = true;
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
	else
	{
		event->accept();
	}

	if(scrollBarEnabled == 0)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else if(scrollBarEnabled == 1)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else if(scrollBarEnabled == 2)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}

}

void SettingsWindow::show(int tab)
{
	tabs->setCurrentIndex(tab);
	QDialog::show();
}

void SettingsWindow::loadSettings()
{
	qDebug("Load Settings . . .");
	windowStartmodus = settings->value(tr("General/windowStartmodus")).toUInt();
	fixedSize = settings->value(tr("General/fixedSize")).toBool();
	fixedHeight = settings->value(tr("General/fixedHeight")).toUInt();
	fixedWidth = settings->value(tr("General/fixedWidth")).toUInt();
	hiddenMenu = settings->value(tr("General/hiddenMenu")).toBool();
	gameNumber = settings->value(tr("General/gameNumber")).toUInt();
	aliveCellColor.setRed(settings->value(tr("General/aliveCellColorRed")).toInt());
	aliveCellColor.setGreen(settings->value(tr("General/aliveCellColorGreen")).toInt());
	aliveCellColor.setBlue(settings->value(tr("General/aliveCellColorBlue")).toInt());
	deadCellColor.setRed(settings->value(tr("General/deadCellColorRed")).toInt());
	deadCellColor.setGreen(settings->value(tr("General/deadCellColorGreen")).toInt());
	deadCellColor.setBlue(settings->value(tr("General/deadCellColorBlue")).toInt());
	lineColor.setRed(settings->value(tr("General/lineColorRed")).toInt());
	lineColor.setGreen(settings->value(tr("General/lineColorGreen")).toInt());
	lineColor.setBlue(settings->value(tr("General/lineColorBlue")).toInt());
	backgroundColor.setRed(settings->value(tr("General/backgroundColorRed")).toInt());
	backgroundColor.setGreen(settings->value(tr("General/backgroundColorGreen")).toInt());
	backgroundColor.setBlue(settings->value(tr("General/backgroundColorBlue")).toInt());
	exitWarning = settings->value(tr("General/exitWarning")).toBool();
	dynSettingChange = settings->value(tr("General/dynSettingChange")).toBool();
	customColorsSize = settings->value(tr("General/customColorsSize")).toUInt();
	customColors.resize(customColorsSize);
	for(unsigned int i = 0; i < customColorsSize; ++i)
	{
		customColors[i].setRed(settings->value(tr("General/customColorsRed") + QString::number(i)).toInt());
		customColors[i].setGreen(settings->value(tr("General/customColorsGreen") + QString::number(i)).toInt());
		customColors[i].setBlue(settings->value(tr("General/customColorsBlue") + QString::number(i)).toInt());
	}
	scrollBarEnabled = settings->value(tr("General/scrollBarEnabled")).toInt();

	autoplaySpeed = settings->value(tr("Game/autoplaySpeed")).toUInt();
	maxAutoplayEnabled = settings->value(tr("Game/maxAutoplayEnabled")).toBool();
	maxAutoplay = settings->value(tr("Game/maxAutoplay")).toInt();
	cellSize = settings->value(tr("Game/cellSize")).toUInt();
	gridWidth = settings->value(tr("Game/gridWidth")).toUInt();
	x = settings->value(tr("Game/x")).toUInt();
	y = settings->value(tr("Game/y")).toUInt();
	timerSize = settings->value(tr("Game/timerSize")).toUInt();
}

void SettingsWindow::setDefaultValues()
{
	qDebug("Set Default Settings . . .");				// default values
	autoplaySpeed = 100;
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
	backgroundColor = Qt::black;
	customColors.resize(QColorDialog::customCount());
	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = Qt::white;
	customColorsSize = QColorDialog::customCount();
	scrollBarEnabled = 0;
}

void SettingsWindow::updateSettings()
{
	fixedSizeCheckbox->setChecked(fixedSize);
	windowStartmodusCombobox->setCurrentIndex(windowStartmodus);
	fixedSizeWidthEnter->setValue(fixedWidth);
	fixedSizeHeightEnter->setValue(fixedHeight);
	hideMenu->setChecked(hiddenMenu);
	gameNumberEnter->setValue(gameNumber);
	exitWarningCheckbox->setChecked(exitWarning);
	enableDynSettingChange->setChecked(dynSettingChange);
	aliveCellColorPalette->setColor(QPalette::Window, aliveCellColor);
	aliveCellColorPreview->setPalette(*aliveCellColorPalette);
	deadCellColorPalette->setColor(QPalette::Window, deadCellColor);
	deadCellColorPreview->setPalette(*deadCellColorPalette);
	lineColorPalette->setColor(QPalette::Window, lineColor);
	lineColorPreview->setPalette(*lineColorPalette);
	backgroundColorPalette->setColor(QPalette::Window, backgroundColor);
	backgroundColorPreview->setPalette(*backgroundColorPalette);
	scrollBarEnabledCombobox->setCurrentIndex(scrollBarEnabled);

	enableMaxAutoplayCheckbox->setChecked(maxAutoplayEnabled);
	maxAutoplaySpinbox->setValue(maxAutoplay);
	autoplaySpeedSpinbox->setValue(autoplaySpeed);
	cellSizeEnter->setValue(cellSize);
	gridWidthEnter->setValue(gridWidth);
	xEnter->setValue(x);
	yEnter->setValue(y);
	timerSizeEnter->setValue(timerSize);
}

void SettingsWindow::setGraphicalWidgets(GraphicalWindow* graphic, Label* label)
{
	this->graphic = graphic;
	this->label = label;
}

void SettingsWindow::createGeneralTab()
{
	generaltab = new QWidget;
	QGridLayout* tabLay = new QGridLayout;

	QGroupBox* windowGroup = new QGroupBox(tr("Window"));
	createWindowGroupbox(windowGroup);

	QGroupBox* otherGroup = new QGroupBox(tr("Other"));
	createOtherGroupbox(otherGroup);

	tabLay->addWidget(windowGroup, 0, 0);
	tabLay->addWidget(otherGroup, 1, 0);

	generaltab->setLayout(tabLay);
	generaltab->show();
}

void SettingsWindow::createWindowGroupbox(QGroupBox* windowGroup)
{
	QVBoxLayout* windowGroupLay = new QVBoxLayout;

	QLabel* windowStartmodusLabel = new QLabel(tr("Startmodus:"));
	windowStartmodusCombobox = new QComboBox;
	windowStartmodusCombobox->addItem(tr("Standard"));
	windowStartmodusCombobox->addItem(tr("Maximized"));
	windowStartmodusCombobox->addItem(tr("Minimized"));
	windowStartmodusCombobox->addItem(tr("Fullscreen"));
	QObject::connect(windowStartmodusCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setNewWindowStartmodus(int)));
	windowStartmodusCombobox->setCurrentIndex(windowStartmodus);

	fixedSizeCheckbox = new QCheckBox(tr("Fixed Window Size"));
	QObject::connect(fixedSizeCheckbox, SIGNAL(toggled(bool)), this, SLOT(fixedSizeActivated(bool)));
	QLabel* fixedSizeWidthLabel = new QLabel(tr("Fixed Width:"));
	fixedSizeWidthEnter = new QSpinBox;
	fixedSizeWidthEnter->setMinimum(100);
	fixedSizeWidthEnter->setMaximum(1920);
	QObject::connect(fixedSizeWidthEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewFixedWidth(int)));
	fixedSizeWidthEnter->setValue(fixedWidth);
	QLabel* fixedSizeHeightLabel = new QLabel(tr("Fixed Height: "));
	fixedSizeHeightEnter = new QSpinBox;
	fixedSizeHeightEnter->setMinimum(100);
	fixedSizeHeightEnter->setMaximum(1080);
	QObject::connect(fixedSizeHeightEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewFixedHeight(int)));
	fixedSizeHeightEnter->setValue(fixedHeight);

	windowGroupLay->addWidget(windowStartmodusLabel);
	windowGroupLay->addWidget(windowStartmodusCombobox);
	windowGroupLay->addWidget(fixedSizeCheckbox);
	windowGroupLay->addWidget(fixedSizeWidthLabel);
	windowGroupLay->addWidget(fixedSizeWidthEnter);
	windowGroupLay->addWidget(fixedSizeHeightLabel);
	windowGroupLay->addWidget(fixedSizeHeightEnter);

	fixedSizeCheckbox->setChecked(fixedSize);
	if(!fixedSize)
		fixedSizeActivated(fixedSize);

	windowGroup->setLayout(windowGroupLay);
}

void SettingsWindow::createOtherGroupbox(QGroupBox* otherGroup)
{
	QGridLayout* otherGroupLayout = new QGridLayout;

	hideMenu = new QCheckBox(tr("Hide menubar"));
	QObject::connect(hideMenu, SIGNAL(clicked(bool)), this, SLOT(hideMenuActivated(bool)));
	hideMenu->setChecked(hiddenMenu);

	QLabel* scrollBarEnabledLabel = new QLabel;
	scrollBarEnabledLabel->setText(tr("Scrollbar behaviour: "));
	scrollBarEnabledCombobox = new QComboBox;
	scrollBarEnabledCombobox->addItem(tr("Always Off"));
	scrollBarEnabledCombobox->addItem(tr("Always On"));
	scrollBarEnabledCombobox->addItem(tr("As Needed"));
	scrollBarEnabledCombobox->setCurrentIndex(scrollBarEnabled);
	QObject::connect(scrollBarEnabledCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(newScrollBarPolicy(int)));

	enableDynSettingChange = new QCheckBox(tr("Enable dynamically settings changes"));
	QObject::connect(enableDynSettingChange, SIGNAL(clicked(bool)), this, SLOT(dynSettingChangeEnabled(bool)));
	enableDynSettingChange->setChecked(dynSettingChange);
	QLabel* gameNumberTxt = new QLabel;
	gameNumberTxt->setText(tr("Game Number"));
	gameNumberEnter = new QSpinBox;
	gameNumberEnter->setValue(gameNumber);
	QObject::connect(gameNumberEnter, SIGNAL(valueChanged(int)), this, SLOT(setGameNumber(int)));

	exitWarningCheckbox = new QCheckBox(tr("Enable exit warning"));
	QObject::connect(exitWarningCheckbox, SIGNAL(clicked(bool)), this, SLOT(exitWarningActivated(bool)));
	exitWarningCheckbox->setChecked(exitWarning);

	resetButton = new QPushButton(tr("Reset Settings"));
	QObject::connect(resetButton, SIGNAL(clicked(bool)), this, SLOT(resetSettings()));

	otherGroupLayout->addWidget(hideMenu, 0, 0);
	otherGroupLayout->addWidget(enableDynSettingChange, 1, 0);
	otherGroupLayout->addWidget(exitWarningCheckbox, 2, 0);
	otherGroupLayout->addWidget(scrollBarEnabledLabel, 3, 0);
	otherGroupLayout->addWidget(scrollBarEnabledCombobox, 3, 1);
	otherGroupLayout->addWidget(gameNumberTxt, 4, 0);
	otherGroupLayout->addWidget(gameNumberEnter, 4, 1);
	otherGroupLayout->addWidget(resetButton, 5, 3);

	otherGroup->setLayout(otherGroupLayout);
}

void SettingsWindow::createColorsGroupbox(QGroupBox* colorsGroup)
{
	QGridLayout* colorsGroupLayout = new QGridLayout;

	QLabel* aliveCellColorLabel = new QLabel;
	aliveCellColorLabel->setText(tr("Color of living Cells:"));
	aliveCellColorPalette = new QPalette(aliveCellColor);
	aliveCellColorButton = new QPushButton(tr("Change"));
//	aliveCellColorButton->setPalette(*aliveCellColorPalette);
//	aliveCellColorButton->setFlat(true);
	aliveCellColorPreview = new QLabel;
	aliveCellColorPreview->setPalette(*aliveCellColorPalette);
	aliveCellColorPreview->setAutoFillBackground(true);
	QObject::connect(aliveCellColorButton, SIGNAL(clicked(bool)), this, SLOT(newAliveCellColor()));

	QLabel* deadCellColorLabel = new QLabel;
	deadCellColorLabel->setText(tr("Color of dead Cells:"));
	deadCellColorPalette = new QPalette(deadCellColor);
	deadCellColorButton = new QPushButton(tr("Change"));
	deadCellColorPreview = new QLabel;
	deadCellColorPreview->setPalette(*deadCellColorPalette);
	deadCellColorPreview->setAutoFillBackground(true);
	QObject::connect(deadCellColorButton, SIGNAL(clicked(bool)), this, SLOT(newDeadCellColor()));

	QLabel* lineColorLabel = new QLabel;
	lineColorLabel->setText(tr("Color of the lines:"));
	lineColorPalette = new QPalette(lineColor);
	lineColorButton = new QPushButton(tr("Change"));
	lineColorPreview = new QLabel;
	lineColorPreview->setPalette(*lineColorPalette);
	lineColorPreview->setAutoFillBackground(true);
	QObject::connect(lineColorButton, SIGNAL(clicked(bool)), this, SLOT(newLineColor()));

	QLabel* backgroundColorLabel = new QLabel;
	backgroundColorLabel->setText(tr("Background color:"));
	backgroundColorPalette = new QPalette(backgroundColor);
	backgroundColorButton = new QPushButton(tr("Change"));
	backgroundColorPreview = new QLabel;
	backgroundColorPreview->setPalette(*backgroundColorPalette);
	backgroundColorPreview->setAutoFillBackground(true);
	QObject::connect(backgroundColorButton, SIGNAL(clicked(bool)), this, SLOT(newBackgroundColor()));

	colorsGroupLayout->addWidget(aliveCellColorLabel, 0, 0);
	colorsGroupLayout->addWidget(aliveCellColorPreview, 0, 1);
	colorsGroupLayout->addWidget(aliveCellColorButton, 0, 2);
	colorsGroupLayout->addWidget(deadCellColorLabel, 1, 0);
	colorsGroupLayout->addWidget(deadCellColorPreview, 1, 1);
	colorsGroupLayout->addWidget(deadCellColorButton, 1, 2);
	colorsGroupLayout->addWidget(lineColorLabel, 2, 0);
	colorsGroupLayout->addWidget(lineColorPreview, 2, 1);
	colorsGroupLayout->addWidget(lineColorButton, 2, 2);
	colorsGroupLayout->addWidget(backgroundColorLabel, 3, 0);
	colorsGroupLayout->addWidget(backgroundColorPreview, 3, 1);
	colorsGroupLayout->addWidget(backgroundColorButton, 3, 2);

	colorsGroup->setLayout(colorsGroupLayout);
}

void SettingsWindow::createGameTab()
{
	gametab = new QWidget;
	QGridLayout* tabLay = new QGridLayout;

	QGroupBox* autoplayBox = new QGroupBox(tr("Autoplay"));
	createAutoplayGroupbox(autoplayBox);

	QGroupBox* areaBox = new QGroupBox(tr("Area"));
	createAreaGroupbox(areaBox);

	QGroupBox* colorsGroup = new QGroupBox(tr("Colors"));
	createColorsGroupbox(colorsGroup);

	tabLay->addWidget(autoplayBox, 1, 2);
	tabLay->addWidget(colorsGroup, 0, 2);
	tabLay->addWidget(areaBox, 0, 1, 2, 1);

	gametab->setLayout(tabLay);
	gametab->show();
}

void SettingsWindow::createAutoplayGroupbox(QGroupBox* autoplayBox)
{
	QGridLayout* autoplayBoxLayout = new QGridLayout;

	QLabel* autoplaySpeedTxt = new QLabel;
	autoplaySpeedTxt->setText(tr("Time between two generations: "));
	autoplaySpeedSpinbox = new QSpinBox;
	autoplaySpeedSpinbox->setMinimum(0);
	autoplaySpeedSpinbox->setMaximum(10000);
	autoplaySpeedSpinbox->setValue(autoplaySpeed);
	QObject::connect(autoplaySpeedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(setNewAutoplaySpeed(int)));
	QLabel* autoplaySpeedUnit = new QLabel;
	autoplaySpeedUnit->setText(tr("milliseconds"));

	enableMaxAutoplayCheckbox = new QCheckBox(tr("Enable maximal generations by autoplay"));
	enableMaxAutoplayCheckbox->setChecked(maxAutoplayEnabled);
	QObject::connect(enableMaxAutoplayCheckbox, SIGNAL(clicked(bool)), this, SLOT(maxAutoplayActivated(bool)));
	QLabel* maxAutoplayTxt = new QLabel;
	maxAutoplayTxt->setText(tr("Maximal generations by autoplay: "));
	maxAutoplaySpinbox = new QSpinBox;
	maxAutoplaySpinbox->setMinimum(1);
	maxAutoplaySpinbox->setMaximum(std::numeric_limits<decltype(maxAutoplay)>::max());
	maxAutoplaySpinbox->setValue(maxAutoplay);
	QObject::connect(maxAutoplaySpinbox, SIGNAL(valueChanged(int)), this, SLOT(setNewMaxAutoplay(int)));

	if(enableMaxAutoplayCheckbox->isChecked())
	{
		maxAutoplaySpinbox->setEnabled(true);
	}
	else
	{
		maxAutoplaySpinbox->setDisabled(true);
	}

	autoplayBoxLayout->addWidget(autoplaySpeedTxt, 0, 1);
	autoplayBoxLayout->addWidget(autoplaySpeedSpinbox, 0, 2);
	autoplayBoxLayout->addWidget(autoplaySpeedUnit, 0, 3);
	autoplayBoxLayout->addWidget(enableMaxAutoplayCheckbox, 1, 1);
	autoplayBoxLayout->addWidget(maxAutoplayTxt, 2, 1);
	autoplayBoxLayout->addWidget(maxAutoplaySpinbox, 3, 1);

	autoplayBox->setLayout(autoplayBoxLayout);
}

void SettingsWindow::createAreaGroupbox(QGroupBox* areaBox)
{
	QGridLayout* areaBoxLayout = new QGridLayout;

	QLabel* cellSizeTxt = new QLabel;
	cellSizeTxt->setText(tr("Cell size: "));
	cellSizeEnter = new QSpinBox;
	cellSizeEnter->setMinimum(1);
	cellSizeEnter->setMaximum(1000);
	QObject::connect(cellSizeEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewCellSize(int)));
	cellSizeEnter->setValue(cellSize);
	QLabel* cellSizeUnit = new QLabel;
	cellSizeUnit->setText(tr("px"));

	QLabel* gridWidthTxt = new QLabel;
	gridWidthTxt->setText(tr("Line thickness: "));
	gridWidthEnter = new QSpinBox;
	gridWidthEnter->setMinimum(1);
	gridWidthEnter->setMaximum(500);
	QObject::connect(gridWidthEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewGridWidth(int)));
	gridWidthEnter->setValue(gridWidth);
	QLabel* gridWidthUnit = new QLabel;
	gridWidthUnit->setText(tr("px"));

	QLabel* xTxt = new QLabel;
	xTxt->setText(tr("x: "));
	xEnter = new QSpinBox;
	xEnter->setMinimum(2);
	xEnter->setMaximum(10000);
	QObject::connect(xEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewX(int)));
	xEnter->setValue(x);
	QLabel* xUnit = new QLabel;
	xUnit->setText(tr("Cells"));

	QLabel* yTxt = new QLabel;
	yTxt->setText(tr("y: "));
	yEnter = new QSpinBox;
	yEnter->setMinimum(2);
	yEnter->setMaximum(10000);
	QObject::connect(yEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewY(int)));
	yEnter->setValue(y);
	QLabel* yUnit = new QLabel;
	yUnit->setText(tr("Cells"));

	QLabel* timerSizeTxt = new QLabel;
	timerSizeTxt->setText(tr("Counter size: "));
	timerSizeEnter = new QSpinBox;
	timerSizeEnter->setMinimum(5);
	timerSizeEnter->setMaximum(200);
	QObject::connect(timerSizeEnter, SIGNAL(valueChanged(int)), this, SLOT(setNewTimerSize(int)));
	timerSizeEnter->setValue(timerSize);
	QLabel* timerSizeUnit = new QLabel;
	timerSizeUnit->setText(tr("px"));

	areaBoxLayout->addWidget(cellSizeTxt, 0, 0);
	areaBoxLayout->addWidget(cellSizeEnter, 0, 1);
	areaBoxLayout->addWidget(cellSizeUnit, 0, 2);
	areaBoxLayout->addWidget(gridWidthTxt, 1, 0);
	areaBoxLayout->addWidget(gridWidthEnter, 1, 1);
	areaBoxLayout->addWidget(gridWidthUnit, 1, 2);
	areaBoxLayout->addWidget(xTxt, 2, 0);
	areaBoxLayout->addWidget(xEnter, 2, 1);
	areaBoxLayout->addWidget(xUnit, 2, 2);
	areaBoxLayout->addWidget(yTxt, 3, 0);
	areaBoxLayout->addWidget(yEnter, 3, 1);
	areaBoxLayout->addWidget(yUnit, 3, 2);
	areaBoxLayout->addWidget(timerSizeTxt, 4, 0);
	areaBoxLayout->addWidget(timerSizeEnter, 4, 1);
	areaBoxLayout->addWidget(timerSizeUnit, 4, 2);

	areaBox->setLayout(areaBoxLayout);
}

void SettingsWindow::createShortcuts()
{
	QAction* closeSettings = new QAction;
	closeSettings->setShortcut(QKeySequence(Qt::Key_Escape));
	QObject::connect(closeSettings, SIGNAL(triggered(bool)), this, SLOT(close()));
	this->addAction(closeSettings);

	QAction* saveChanges = new QAction;
	saveChanges->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	QObject::connect(saveChanges, SIGNAL(triggered(bool)), this, SLOT(saveSettings()));
	this->addAction(saveChanges);

	QAction* showGeneralTab = new QAction;
	showGeneralTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
	QObject::connect(showGeneralTab, SIGNAL(triggered(bool)), this, SLOT(showGeneral()));
	this->addAction(showGeneralTab);

	QAction* showGameTab = new QAction;
	showGameTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	QObject::connect(showGameTab, SIGNAL(triggered(bool)), this, SLOT(showGame()));
	this->addAction(showGameTab);
}

void SettingsWindow::setNewAutoplaySpeed(int newValue)
{
	saved = false;

	autoplaySpeed = newValue;
}

void SettingsWindow::maxAutoplayActivated(bool activated)
{
	saved = false;

	maxAutoplayEnabled = activated;

	if(activated)
	{
		maxAutoplaySpinbox->setDisabled(false);
	}
	else
	{
		maxAutoplaySpinbox->setDisabled(true);
	}
}

void SettingsWindow::setNewMaxAutoplay(int newValue)
{
	saved = false;

	maxAutoplay = newValue;
}

void SettingsWindow::setNewWindowStartmodus(int newValue)
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

void SettingsWindow::fixedSizeActivated(bool activated)
{
	saved = false;

	if(activated)
	{
		windowStartmodusCombobox->setDisabled(true);
		fixedSizeWidthEnter->setDisabled(false);
		fixedSizeHeightEnter->setDisabled(false);

		fixedSize = true;
	}
	else
	{
		windowStartmodusCombobox->setDisabled(false);
		fixedSizeWidthEnter->setDisabled(true);
		fixedSizeHeightEnter->setDisabled(true);

		fixedSize = false;
	}
}

void SettingsWindow::setNewFixedWidth(int newValue)
{
	saved = false;

	fixedWidth = newValue;
}

void SettingsWindow::setNewFixedHeight(int newValue)
{
	saved = false;

	fixedHeight = newValue;
}

void SettingsWindow::hideMenuActivated(bool activated)
{
	saved = false;

	hiddenMenu = activated;
}

void SettingsWindow::exitWarningActivated(bool activated)
{
	saved = false;

	exitWarning = activated;
}

void SettingsWindow::setNewCellSize(int newValue)
{
	saved = false;

	cellSize = newValue;
}

void SettingsWindow::setNewGridWidth(int newValue)
{
	saved = false;

	gridWidth = newValue;
}

void SettingsWindow::setNewX(int newValue)
{
	saved = false;

	x = newValue;
}

void SettingsWindow::setNewY(int newValue)
{
	saved = false;

	y = newValue;
}

void SettingsWindow::setNewTimerSize(int newValue)
{
	saved = false;

	timerSize = newValue;
}

void SettingsWindow::setGameNumber(int newValue)
{
	saved = false;

	gameNumber = newValue;
}

void SettingsWindow::newAliveCellColor()
{
/*	QColorDialog colordia;
	colordia.setCurrentColor(aliveCellColor.color());
	colordia.show();
	aliveCellColor = colordia.currentColor();*/

	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	aliveCellColor = QColorDialog::getColor(aliveCellColor, this, "Alive Cell Color");

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	aliveCellColorPalette->setColor(QPalette::Window, aliveCellColor);
	aliveCellColorPreview->setPalette(*aliveCellColorPalette);

	if(dynSettingChange)
		graphic->scene->update(graphic->sceneRect());
}

void SettingsWindow::newDeadCellColor()
{
/*	QColorDialog colordia;
	colordia.setCurrentColor(deadCellColor.color());
	colordia.show();
	deadCellColor = colordia.currentColor();*/

	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	deadCellColor = QColorDialog::getColor(deadCellColor, this, "Dead Cell Color");

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	deadCellColorPalette->setColor(QPalette::Window, deadCellColor);
	deadCellColorPreview->setPalette(*deadCellColorPalette);

	if(dynSettingChange)
		graphic->scene->update(graphic->sceneRect());
}

void SettingsWindow::newLineColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	lineColor = QColorDialog::getColor(lineColor, this, "Line Color");

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	lineColorPalette->setColor(QPalette::Window, lineColor);
	lineColorPreview->setPalette(*lineColorPalette);

	if(dynSettingChange)
		graphic->scene->update(graphic->sceneRect());
}

void SettingsWindow::newBackgroundColor()
{
	saved = false;

	for(int i = 0; i < customColors.size(); ++i)
		QColorDialog::setCustomColor(i, customColors[i]);

	backgroundColor = QColorDialog::getColor(backgroundColor, this, "Background Color");

	for(int i = 0; i < customColors.size(); ++i)
		customColors[i] = QColorDialog::customColor(i);

	backgroundColorPalette->setColor(QPalette::Window, backgroundColor);
	backgroundColorPreview->setPalette(*backgroundColorPalette);

	if(dynSettingChange)
		graphic->setBackgroundBrush(backgroundColor);
}

void SettingsWindow::dynSettingChangeEnabled(bool activated)
{
	saved = false;

	dynSettingChange = activated;
}

void SettingsWindow::newScrollBarPolicy(int newValue)
{
	saved = false;

	// 0 = Always Off
	// 1 = Always On
	// 2 = As Needed

	scrollBarEnabled = newValue;

	if(dynSettingChange && scrollBarEnabled == 0)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else if(dynSettingChange && scrollBarEnabled == 1)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else if(dynSettingChange && scrollBarEnabled == 2)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}

void SettingsWindow::resetSettings()
{
	QMessageBox::Button btn = QMessageBox::warning(this, "Reset Settings", "Are you sure?", QMessageBox::Yes | QMessageBox::No);

	if(btn == QMessageBox::Yes)
	{
		this->setDefaultValues();
		this->saveSettings();
		this->updateSettings();
	}
}

void SettingsWindow::saveGame()
{
	qDebug("save...");
	QString filename = QString::number(gameNumber);
	filename += "_SAVEFILE-GAME_OF_LIFE.save";

	if(!QDir("save").exists())
		QDir().mkdir("save");

	while(QFile("save/" + filename).exists())
	{
		unsigned int temp = gameNumber + 1;
		filename.replace(QString::number(gameNumber), QString::number(temp));
		++gameNumber;
		QMessageBox::warning(this, "Override Save?", filename + " exists already!\nCreate new Save or override old Save?", QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No);
	}
	filename.insert(0, "save/");

	QFile file(filename);
	if(file.open(QIODevice::Truncate | QIODevice::WriteOnly))
	{
		QDataStream stream(&file);

		stream << static_cast<int>(graphic->getGeneration())
			<< static_cast<int>(graphic->getSystem().size()) - 1
			<< static_cast<int>(graphic->getSystem().back().size()) - 1;

		for(auto &a : graphic->getSystem())
		{
			for(auto &b : a)
			{
				stream << static_cast<int>(b.isAlive());
			}
		}
	}
	else
		QMessageBox::warning(this, "ERROR", "Game can not be saved!", QMessageBox::Ok);

	settings->beginGroup("General");
	settings->setValue("gameNumber", ++gameNumber);
	settings->endGroup();
}

void SettingsWindow::loadGame()
{
	QString selectedFilter("Savefiles (*.save)");
	std::string filename = QFileDialog::getOpenFileName(this, tr("Select Savefile..."), QDir::currentPath() + "/save", "All Files (*.*);; Savefiles (*.save)", &selectedFilter).toStdString();
	if(filename.size() > 5)
	{
		if(filename.substr(filename.find_last_of('.')) == ".save")
		{
			readSavefile(new QFile(QString(filename.c_str())));
		}
		else
		{
			QMessageBox::information(this, "Invalid File", QString(filename.c_str()) + " can not be opened!");
		}
	}
	else if(filename.size() == 0)
	{

	}
	else
	{
		QMessageBox::information(this, "Invalid File", QString(filename.c_str()) + " is invalid!");
	}
}

void SettingsWindow::readSavefile(QFile* file)
{
	if(file->open(QIODevice::ReadOnly))
	{
		qDebug("interpret file...");
		int generation, w, h;
		int alive;

		QDataStream stream(file);

		stream >> generation >> w >> h;
		std::vector<Cell> row;
		graphic->getOriginalSystem() = GraphicalWindow::defaultVector;

		for(int a = 0; a <= w; ++a)
		{
			for(int b = 0; b <= h; ++b)
			{
				stream >> alive;
			//	alive = 1;

				if(alive)
					row.push_back(Cell(graphic->getOriginalSystem(), true, b, a, w, h));
				else
					row.push_back(Cell(graphic->getOriginalSystem(), false, b, a, w, h));

				row.back().getOriginalGeneration() = generation;
			}

			graphic->getOriginalSystem().push_back(std::move(row));
			row.clear();
		}

		graphic->setCellScene(graphic->scene, w + 1, h + 1, graphic->getOriginalSystem());
		label->changeTimer(generation);
	}
	else
	{
		QMessageBox::information(this, "No Reading Rights!", "File is not readable!", QMessageBox::Cancel);
	}

	delete file;
}

void SettingsWindow::saveSettings()
{
	if(!saved && settings->isWritable())
	{
		qDebug("Save Settings . . .");

		settings->beginGroup(tr("General"));
		settings->setValue(tr("windowStartmodus"), windowStartmodus);
		settings->setValue(tr("fixedSize"), fixedSize);
		settings->setValue(tr("fixedWidth"), fixedWidth);
		settings->setValue(tr("fixedHeight"), fixedHeight);
		settings->setValue(tr("hiddenMenu"), hiddenMenu);
		settings->setValue(tr("gameNumber"), gameNumber);
		settings->setValue(tr("aliveCellColorRed"), aliveCellColor.red());
		settings->setValue(tr("aliveCellColorGreen"), aliveCellColor.green());
		settings->setValue(tr("aliveCellColorBlue"), aliveCellColor.blue());
		settings->setValue(tr("deadCellColorRed"), deadCellColor.red());
		settings->setValue(tr("deadCellColorGreen"), deadCellColor.green());
		settings->setValue(tr("deadCellColorBlue"), deadCellColor.blue());
		settings->setValue(tr("lineColorRed"), lineColor.red());
		settings->setValue(tr("lineColorGreen"), lineColor.green());
		settings->setValue(tr("lineColorBlue"), lineColor.blue());
		settings->setValue(tr("backgroundColorRed"), backgroundColor.red());
		settings->setValue(tr("backgroundColorGreen"), backgroundColor.green());
		settings->setValue(tr("backgroundColorBlue"), backgroundColor.blue());
		settings->setValue(tr("exitWarning"), exitWarning);
		settings->setValue(tr("dynSettingChange"), dynSettingChange);
		settings->setValue(tr("customColorsSize"), customColorsSize);
		for(int i = 0; i < customColors.size(); ++i)
		{
			settings->setValue(tr(QString("customColorsRed" + QString::number(i)).toStdString().c_str()), customColors[i].red());
			settings->setValue(tr(QString("customColorsGreen" + QString::number(i)).toStdString().c_str()), customColors[i].green());
			settings->setValue(tr(QString("customColorsBlue" + QString::number(i)).toStdString().c_str()), customColors[i].blue());
		}
		settings->setValue(tr("scrollBarEnabled"), scrollBarEnabled);
		settings->endGroup();
		settings->beginGroup(tr("Game"));
		settings->setValue(tr("autoplaySpeed"), autoplaySpeed);
		settings->setValue(tr("maxAutoplayEnabled"), maxAutoplayEnabled);
		settings->setValue(tr("maxAutoplay"), maxAutoplay);
		settings->setValue(tr("cellSize"), cellSize);
		settings->setValue(tr("gridWidth"), gridWidth);
		settings->setValue(tr("x"), x);
		settings->setValue(tr("y"), y);
		settings->setValue(tr("timerSize"), timerSize);
		settings->endGroup();
	}

	saved = true;
}
