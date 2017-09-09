#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "settings.h"
#include <QDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

class QGridLayout;
class QGroupBox;
class QCloseEvent;
class QTabWidget;
class QLabel;

class GraphicalWindow;
class Label;
class SeparateThread;

class SettingsWindow : public QDialog
{
	Q_OBJECT

	friend class Settings;

	Settings* setts;

	GraphicalWindow* graphic;
	Label* label;

	QGridLayout* grid;

	QTabWidget* tabs;
	QWidget* generaltab;
	QWidget* gametab;

	void createGeneralTab();
		void createWindowGroupbox(QGroupBox* windowGroup);
		void createOtherGroupbox(QGroupBox* otherGroup);
		void createColorsGroupbox(QGroupBox* colorsGroup);

	void createGameTab();
		void createAutoplayGroupbox(QGroupBox* autoplayBox);
		void createAreaGroupbox(QGroupBox* areaBox);

	void createShortcuts();

	QCheckBox* fixedSizeCheckbox;
	QComboBox* windowStartmodusCombobox;
	QSpinBox* fixedSizeWidthEnter;
	QSpinBox* fixedSizeHeightEnter;
	QCheckBox* hideMenu;
	QCheckBox* exitWarningCheckbox;
	QCheckBox* enableDynSettingChange;
	QSpinBox* gameNumberEnter;
	QPushButton* resetButton;
	QCheckBox* enableMaxAutoplayCheckbox;
	QSpinBox* maxAutoplaySpinbox;
	QSpinBox* autoplaySpeedSpinbox;
	QSpinBox* cellSizeEnter;
	QSpinBox* gridWidthEnter;
	QSpinBox* xEnter;
	QSpinBox* yEnter;
	QSpinBox* timerSizeEnter;
	QLabel* aliveCellColorPreview;
	QPushButton* aliveCellColorButton;
	QLabel* deadCellColorPreview;
	QPushButton* deadCellColorButton;
	QLabel* lineColorPreview;
	QPushButton* lineColorButton;
	QLabel* nextGenAliveCellColorPreview;
	QPushButton* nextGenAliveCellColorButton;
	QLabel* nextGenDeadCellColorPreview;
	QPushButton* nextGenDeadCellColorButton;
	QLabel* backgroundColorPreview;
	QPushButton* backgroundColorButton;
	QComboBox* scrollBarEnabledCombobox;

	void closeEvent(QCloseEvent* event);

public:
	explicit SettingsWindow(QWidget* parent = nullptr);

	void setGraphicalWidgets(GraphicalWindow* graphic, Label* label);

	Settings* settings() { return setts; }

public slots:
	void show(int tab = 0);
	void showGeneral() { show(0); }
	void showGame() { show(1); }
	void setDefaultValues() { setts->setDefaultValues(); }
	void updateSettings();
	void resetSettings();
};

#endif // SETTINGSWINDOW_H
