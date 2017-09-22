#include "settingswindow.h"
#include "settings.h"
#include "graphicalwindow.h"
#include "separatethread.h"
#include "label.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QCloseEvent>
#include <QAction>

#include <climits>

SettingsWindow::SettingsWindow(QWidget* parent) : QDialog(parent)
{
	setts = new Settings(this);

	graphic = nullptr;
	label = nullptr;

	this->setWindowTitle(tr("Settings"));
	this->setWindowIcon(QIcon(":/images/preferences-system.png"));
	this->setWindowModality(Qt::ApplicationModal);			// blocks other windows
	this->setWindowFlags(Qt::WindowCloseButtonHint);		// shows only close button

	tabs = new QTabWidget;
	createGeneralTab();
	tabs->addTab(generaltab, tr("General"));
	createGameTab();
	tabs->addTab(gametab, tr("Game"));

	QDialogButtonBox* diabutts = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close);
	QObject::connect(diabutts->button(QDialogButtonBox::Save), &QPushButton::clicked, setts, &Settings::saveSettings);
	QObject::connect(diabutts->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &SettingsWindow::close);

	QPushButton* restartButton = new QPushButton(tr("Restart"));
	restartButton->setVisible(false);
	QPushButton* englishButton = new QPushButton(QIcon(":/images/gb_flag.png"), "");
	englishButton->setCheckable(true);
	englishButton->setChecked(setts->getLanguage() == Settings::Language::English);
	QObject::connect(englishButton, &QPushButton::clicked, [this]() { this->setts->setLanguage(static_cast<int>(Settings::Language::English)); } );
	QPushButton* germanButton = new QPushButton(QIcon(":/images/de_flag.png"), "");
	germanButton->setCheckable(true);
	germanButton->setChecked(setts->getLanguage() == Settings::Language::German);
	QObject::connect(germanButton, &QPushButton::clicked, [this]() { this->setts->setLanguage(static_cast<int>(Settings::Language::German)); } );
	QObject::connect(englishButton, &QPushButton::clicked, [germanButton, restartButton]() { germanButton->setChecked(false); restartButton->setVisible(true); } );
	QObject::connect(germanButton, &QPushButton::clicked, [englishButton, restartButton]() { englishButton->setChecked(false); restartButton->setVisible(true); } );
	QObject::connect(restartButton, &QPushButton::clicked, [this]() { this->close(); emit this->setts->languageChanged(this->setts->getLanguage()); } );

	createShortcuts();

	grid = new QGridLayout;

	grid->addWidget(tabs, 0, 0, 1, 5);
	grid->addWidget(englishButton, 3, 0);
	grid->addWidget(germanButton, 3, 1);
	grid->addWidget(restartButton, 3, 2);
	grid->addWidget(diabutts, 3, 3);

	this->setLayout(grid);

	setts->saved = true;
}

void SettingsWindow::closeEvent(QCloseEvent* event)
{
	if(!setts->saved)
	{
		QMessageBox::Button btn = QMessageBox::question(this, tr("Save?"), tr("Do you want to save changes?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

		if(btn == QMessageBox::Yes)
		{
			setts->saveSettings();
			setts->saved = true;
			event->accept();
		}
		else if(btn == QMessageBox::No)
		{
			if(setts->setting()->status() == QSettings::NoError && setts->setting()->contains("General/windowStartmodus"))
				setts->loadSettings();
			else
				setts->setDefaultValues();
			updateSettings();
			setts->saved = true;
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

	if(setts->scrollBarEnabled == 0)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else if(setts->scrollBarEnabled == 1)
	{
		graphic->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		graphic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else if(setts->scrollBarEnabled == 2)
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

void SettingsWindow::updateSettings()
{
	fixedSizeCheckbox->setChecked(setts->fixedSize);
	windowStartmodusCombobox->setCurrentIndex(setts->windowStartmodus);
	fixedSizeWidthEnter->setValue(setts->fixedWidth);
	fixedSizeHeightEnter->setValue(setts->fixedHeight);
	hideMenu->setChecked(setts->hiddenMenu);
	gameNumberEnter->setValue(setts->gameNumber);
	exitWarningCheckbox->setChecked(setts->exitWarning);
	enableDynSettingChange->setChecked(setts->dynSettingChange);
	setts->aliveCellColorPalette->setColor(QPalette::Window, setts->aliveCellColor);
	aliveCellColorPreview->setPalette(*setts->aliveCellColorPalette);
	setts->deadCellColorPalette->setColor(QPalette::Window, setts->deadCellColor);
	deadCellColorPreview->setPalette(*setts->deadCellColorPalette);
	setts->lineColorPalette->setColor(QPalette::Window, setts->lineColor);
	lineColorPreview->setPalette(*setts->lineColorPalette);
	setts->backgroundColorPalette->setColor(QPalette::Window, setts->backgroundColor);
	backgroundColorPreview->setPalette(*setts->backgroundColorPalette);
	scrollBarEnabledCombobox->setCurrentIndex(setts->scrollBarEnabled);

	enableMaxAutoplayCheckbox->setChecked(setts->maxAutoplayEnabled);
	maxAutoplaySpinbox->setValue(setts->maxAutoplay);
	autoplaySpeedSpinbox->setValue(setts->autoplaySpeed);
	cellSizeEnter->setValue(setts->cellSize);
	gridWidthEnter->setValue(setts->gridWidth);
	xEnter->setValue(setts->x);
	yEnter->setValue(setts->y);
	timerSizeEnter->setValue(setts->timerSize);
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

	QLabel* windowStartmodusLabel = new QLabel(tr("Start Mode:"));
	windowStartmodusCombobox = new QComboBox;
	windowStartmodusCombobox->addItem(tr("Standard"));
	windowStartmodusCombobox->addItem(tr("Maximized"));
	windowStartmodusCombobox->addItem(tr("Minimized"));
	windowStartmodusCombobox->addItem(tr("Fullscreen"));
	windowStartmodusCombobox->setCurrentIndex(setts->windowStartmodus);
	QObject::connect(windowStartmodusCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), setts, &Settings::setNewWindowStartmodus);

	fixedSizeCheckbox = new QCheckBox(tr("Fixed Window Size"));
	QObject::connect(fixedSizeCheckbox, &QCheckBox::toggled, setts, &Settings::fixedSizeActivated);
	QLabel* fixedSizeWidthLabel = new QLabel(tr("Fixed Width:"));
	fixedSizeWidthEnter = new QSpinBox;
	fixedSizeWidthEnter->setMinimum(100);
	fixedSizeWidthEnter->setMaximum(1920);
	fixedSizeWidthEnter->setValue(setts->fixedWidth);
	QObject::connect(fixedSizeWidthEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewFixedWidth);
	QLabel* fixedSizeHeightLabel = new QLabel(tr("Fixed Height: "));
	fixedSizeHeightEnter = new QSpinBox;
	fixedSizeHeightEnter->setMinimum(100);
	fixedSizeHeightEnter->setMaximum(1080);
	fixedSizeHeightEnter->setValue(setts->fixedHeight);
	QObject::connect(fixedSizeHeightEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewFixedHeight);


	fixedSizeCheckbox->setChecked(setts->fixedSize);
	if(!setts->fixedSize)
		setts->fixedSizeActivated(setts->fixedSize);

	windowGroupLay->addWidget(windowStartmodusLabel);
	windowGroupLay->addWidget(windowStartmodusCombobox);
	windowGroupLay->addWidget(fixedSizeCheckbox);
	windowGroupLay->addWidget(fixedSizeWidthLabel);
	windowGroupLay->addWidget(fixedSizeWidthEnter);
	windowGroupLay->addWidget(fixedSizeHeightLabel);
	windowGroupLay->addWidget(fixedSizeHeightEnter);

	windowGroup->setLayout(windowGroupLay);
}

void SettingsWindow::createOtherGroupbox(QGroupBox* otherGroup)
{
	QGridLayout* otherGroupLayout = new QGridLayout;

	hideMenu = new QCheckBox(tr("Hide menubar"));
	hideMenu->setChecked(setts->hiddenMenu);
	QObject::connect(hideMenu, &QCheckBox::clicked, setts, &Settings::hideMenuActivated);

	QLabel* scrollBarEnabledLabel = new QLabel;
	scrollBarEnabledLabel->setText(tr("Scrollbar behaviour: "));
	scrollBarEnabledCombobox = new QComboBox;
	scrollBarEnabledCombobox->addItem(tr("Always Off"));
	scrollBarEnabledCombobox->addItem(tr("Always On"));
	scrollBarEnabledCombobox->addItem(tr("As Needed"));
	scrollBarEnabledCombobox->setCurrentIndex(setts->scrollBarEnabled);
	QObject::connect(scrollBarEnabledCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), setts, &Settings::newScrollBarPolicy);

	enableDynSettingChange = new QCheckBox(tr("Enable dynamically settings changes"));
	enableDynSettingChange->setChecked(setts->dynSettingChange);
	QObject::connect(enableDynSettingChange, &QCheckBox::clicked, setts, &Settings::dynSettingChangeEnabled);

	QLabel* gameNumberTxt = new QLabel;
	gameNumberTxt->setText(tr("Saved Games: "));
	gameNumberEnter = new QSpinBox;
	gameNumberEnter->setValue(setts->gameNumber);
	QObject::connect(gameNumberEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setGameNumber);

	exitWarningCheckbox = new QCheckBox(tr("Enable exit warning"));
	exitWarningCheckbox->setChecked(setts->exitWarning);
	QObject::connect(exitWarningCheckbox, &QCheckBox::clicked, setts, &Settings::exitWarningActivated);

	resetButton = new QPushButton(tr("Reset Settings"));
	QObject::connect(resetButton, &QPushButton::clicked, this, &SettingsWindow::resetSettings);

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

	QLabel* aliveCellColorLabel = new QLabel(tr("Color of living cells:"));
	setts->aliveCellColorPalette = new QPalette(setts->aliveCellColor);
	aliveCellColorButton = new QPushButton(tr("Change"));
	aliveCellColorPreview = new QLabel;
	aliveCellColorPreview->setPalette(*setts->aliveCellColorPalette);
	aliveCellColorPreview->setAutoFillBackground(true);
	QObject::connect(aliveCellColorButton, &QCheckBox::clicked, setts, &Settings::newAliveCellColor);

	QLabel* deadCellColorLabel = new QLabel(tr("Color of dead cells:"));
	setts->deadCellColorPalette = new QPalette(setts->deadCellColor);
	deadCellColorButton = new QPushButton(tr("Change"));
	deadCellColorPreview = new QLabel;
	deadCellColorPreview->setPalette(*setts->deadCellColorPalette);
	deadCellColorPreview->setAutoFillBackground(true);
	QObject::connect(deadCellColorButton, &QCheckBox::clicked, setts, &Settings::newDeadCellColor);

	QLabel* lineColorLabel = new QLabel(tr("Color of the lines:"));
	setts->lineColorPalette = new QPalette(setts->lineColor);
	lineColorButton = new QPushButton(tr("Change"));
	lineColorPreview = new QLabel;
	lineColorPreview->setPalette(*setts->lineColorPalette);
	lineColorPreview->setAutoFillBackground(true);
	QObject::connect(lineColorButton, &QCheckBox::clicked, setts, &Settings::newLineColor);

	QLabel* nextGenAliveCellColorLabel = new QLabel(tr("Color of next generation born cells: "));
	setts->nextGenAliveCellColorPalette = new QPalette(setts->nextGenAliveCellColor);
	nextGenAliveCellColorButton = new QPushButton(tr("Change"));
	nextGenAliveCellColorPreview = new QLabel;
	nextGenAliveCellColorPreview->setPalette(*setts->nextGenAliveCellColorPalette);
	nextGenAliveCellColorPreview->setAutoFillBackground(true);
	QObject::connect(nextGenAliveCellColorButton, &QCheckBox::clicked, setts, &Settings::newNextGenAliveCellColor);

	QLabel* nextGenDeadCellColorLabel = new QLabel(tr("Color of next generation dying cells: "));
	setts->nextGenDeadCellColorPalette = new QPalette(setts->nextGenDeadCellColor);
	nextGenDeadCellColorButton = new QPushButton(tr("Change"));
	nextGenDeadCellColorPreview = new QLabel;
	nextGenDeadCellColorPreview->setPalette(*setts->nextGenDeadCellColorPalette);
	nextGenDeadCellColorPreview->setAutoFillBackground(true);
	QObject::connect(nextGenDeadCellColorButton, &QCheckBox::clicked, setts, &Settings::newNextGenDeadCellColor);

	QLabel* backgroundColorLabel = new QLabel;
	backgroundColorLabel->setText(tr("Background color:"));
	setts->backgroundColorPalette = new QPalette(setts->backgroundColor);
	backgroundColorButton = new QPushButton(tr("Change"));
	backgroundColorPreview = new QLabel;
	backgroundColorPreview->setPalette(*setts->backgroundColorPalette);
	backgroundColorPreview->setAutoFillBackground(true);
	QObject::connect(backgroundColorButton, &QCheckBox::clicked, setts, &Settings::newBackgroundColor);

	colorsGroupLayout->addWidget(aliveCellColorLabel, 0, 0);
	colorsGroupLayout->addWidget(aliveCellColorPreview, 0, 1);
	colorsGroupLayout->addWidget(aliveCellColorButton, 0, 2);
	colorsGroupLayout->addWidget(deadCellColorLabel, 1, 0);
	colorsGroupLayout->addWidget(deadCellColorPreview, 1, 1);
	colorsGroupLayout->addWidget(deadCellColorButton, 1, 2);
	colorsGroupLayout->addWidget(nextGenAliveCellColorLabel, 2, 0);
	colorsGroupLayout->addWidget(nextGenAliveCellColorPreview, 2, 1);
	colorsGroupLayout->addWidget(nextGenAliveCellColorButton, 2, 2);
	colorsGroupLayout->addWidget(nextGenDeadCellColorLabel, 3, 0);
	colorsGroupLayout->addWidget(nextGenDeadCellColorPreview, 3, 1);
	colorsGroupLayout->addWidget(nextGenDeadCellColorButton, 3, 2);
	colorsGroupLayout->addWidget(lineColorLabel, 4, 0);
	colorsGroupLayout->addWidget(lineColorPreview, 4, 1);
	colorsGroupLayout->addWidget(lineColorButton, 4, 2);
	colorsGroupLayout->addWidget(backgroundColorLabel, 5, 0);
	colorsGroupLayout->addWidget(backgroundColorPreview, 5, 1);
	colorsGroupLayout->addWidget(backgroundColorButton, 5, 2);

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

	QLabel* autoplaySpeedTxt = new QLabel(tr("Time between two generations with autoplay: "));
	autoplaySpeedSpinbox = new QSpinBox;
	autoplaySpeedSpinbox->setMinimum(0);
	autoplaySpeedSpinbox->setMaximum(10000);
	autoplaySpeedSpinbox->setValue(setts->autoplaySpeed);
	QObject::connect(autoplaySpeedSpinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewAutoplaySpeed);
	QLabel* autoplaySpeedUnit = new QLabel(tr("milliseconds"));

	enableMaxAutoplayCheckbox = new QCheckBox(tr("Enable maximal generations with autoplay"));
	enableMaxAutoplayCheckbox->setChecked(setts->maxAutoplayEnabled);
	QObject::connect(enableMaxAutoplayCheckbox, &QCheckBox::clicked, setts, &Settings::maxAutoplayActivated);
	QLabel* maxAutoplayTxt = new QLabel(tr("Maximal generations with autoplay: "));
	maxAutoplaySpinbox = new QSpinBox;
	maxAutoplaySpinbox->setMinimum(1);
	maxAutoplaySpinbox->setMaximum(std::numeric_limits<decltype(setts->maxAutoplay)>::max());
	maxAutoplaySpinbox->setValue(setts->maxAutoplay);
	QObject::connect(maxAutoplaySpinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewMaxAutoplay);

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

	QLabel* cellSizeTxt = new QLabel(tr("Cell size: "));
	cellSizeEnter = new QSpinBox;
	cellSizeEnter->setMinimum(1);
	cellSizeEnter->setMaximum(1000);
	cellSizeEnter->setValue(setts->cellSize);
	QObject::connect(cellSizeEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewCellSize);
	QLabel* cellSizeUnit = new QLabel(tr("px"));

	QLabel* gridWidthTxt = new QLabel(tr("Line thickness: "));
	gridWidthEnter = new QSpinBox;
	gridWidthEnter->setMinimum(1);
	gridWidthEnter->setMaximum(500);
	gridWidthEnter->setValue(setts->gridWidth);
	QObject::connect(gridWidthEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewGridWidth);
	QLabel* gridWidthUnit = new QLabel(tr("px"));

	QLabel* xTxt = new QLabel(tr("x: "));
	xEnter = new QSpinBox;
	xEnter->setMinimum(2);
	xEnter->setMaximum(10000);
	xEnter->setValue(setts->x);
	QObject::connect(xEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewX);
	QLabel* xUnit = new QLabel(tr("Cells"));

	QLabel* yTxt = new QLabel(tr("y: "));
	yEnter = new QSpinBox;
	yEnter->setMinimum(2);
	yEnter->setMaximum(10000);
	yEnter->setValue(setts->y);
	QObject::connect(yEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewY);
	QLabel* yUnit = new QLabel(tr("Cells"));

	QLabel* timerSizeTxt = new QLabel(tr("Counter size: "));
	timerSizeEnter = new QSpinBox;
	timerSizeEnter->setMinimum(5);
	timerSizeEnter->setMaximum(200);
	timerSizeEnter->setValue(setts->timerSize);
	QObject::connect(timerSizeEnter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), setts, &Settings::setNewTimerSize);
	QLabel* timerSizeUnit = new QLabel(tr("px"));

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
	QAction* closeSettings = new QAction(this);
	closeSettings->setShortcut(QKeySequence(Qt::Key_Escape));
	QObject::connect(closeSettings, &QAction::triggered, this, &SettingsWindow::close);
	this->addAction(closeSettings);

	QAction* saveChanges = new QAction(this);
	saveChanges->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	QObject::connect(saveChanges, &QAction::triggered, setts, &Settings::saveSettings);
	this->addAction(saveChanges);

	QAction* showGeneralTab = new QAction(this);
	showGeneralTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
	QObject::connect(showGeneralTab, &QAction::triggered, this, &SettingsWindow::showGeneral);
	this->addAction(showGeneralTab);

	QAction* showGameTab = new QAction(this);
	showGameTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	QObject::connect(showGameTab, &QAction::triggered, this, &SettingsWindow::showGame);
	this->addAction(showGameTab);
}

void SettingsWindow::resetSettings()
{
	QMessageBox::Button btn = QMessageBox::warning(this, tr("Reset Settings"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No);

	if(btn == QMessageBox::Yes)
	{
		setts->setDefaultValues();
		setts->saveSettings();
		this->updateSettings();
	}
}
