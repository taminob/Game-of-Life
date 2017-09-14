#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSettings>
#include <QGridLayout>
#include <QGroupBox>
#include <QCloseEvent>
#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QFile>

class GraphicalWindow;
class Label;

class SettingsWindow : public QDialog
{
	Q_OBJECT

	GraphicalWindow* graphic;
	Label* label;

	QSettings* settings;

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
	QPalette* aliveCellColorPalette;
	QLabel* deadCellColorPreview;
	QPushButton* deadCellColorButton;
	QPalette* deadCellColorPalette;
	QLabel* lineColorPreview;
	QPushButton* lineColorButton;
	QPalette* lineColorPalette;
	QLabel* backgroundColorPreview;
	QPushButton* backgroundColorButton;
	QPalette* backgroundColorPalette;
	QComboBox* scrollBarEnabledCombobox;

	void closeEvent(QCloseEvent *event);

	bool saved;

public:
	explicit SettingsWindow(QWidget *parent = 0);
	~SettingsWindow();

	void setGraphicalWidgets(GraphicalWindow *graphic, Label* label);

public slots:
	void show(int tab = 0);
	void showGeneral() { show(0); }
	void showGame() { show(1); }
	void saveSettings();
	void saveGame();
	void loadSettings();
	void loadGame();
	void readSavefile(QFile*file);
	void setDefaultValues();
	void updateSettings();
	void resetSettings();

private slots:
	void setNewAutoplaySpeed(int newValue);
	void maxAutoplayActivated(bool);
	void setNewMaxAutoplay(int newValue);
	void setNewWindowStartmodus(int newValue);
	void fixedSizeActivated(bool);
	void setNewFixedWidth(int newValue);
	void setNewFixedHeight(int newValue);
	void hideMenuActivated(bool);
	void exitWarningActivated(bool);
	void setNewCellSize(int newValue);
	void setNewGridWidth(int newValue);
	void setNewX(int newValue);
	void setNewY(int newValue);
	void setNewTimerSize(int newValue);
	void setGameNumber(int newValue);
	void newAliveCellColor();
	void newDeadCellColor();
	void newLineColor();
	void newBackgroundColor();
	void dynSettingChangeEnabled(bool);
	void newScrollBarPolicy(int);

private:	// Settings
	unsigned int autoplaySpeed;				// new Attributs need:
	bool maxAutoplayEnabled;				// - attribute
	int maxAutoplay;						// - get-Member
	unsigned int cellSize;					// - slot
	unsigned int gridWidth;					// - default-Value
	unsigned int x;							// - write Settings
	unsigned int y;							// - load Settings
	unsigned int windowStartmodus;			// - private Member: Check-/Spin-/Combobox
	bool fixedSize;							// - update Settings
	unsigned int fixedWidth;
	unsigned int fixedHeight;
	bool hiddenMenu;
	bool exitWarning;
	bool dynSettingChange;
	unsigned int timerSize;
	unsigned int gameNumber;
	QColor aliveCellColor;
	QColor deadCellColor;
	QColor lineColor;
	QColor backgroundColor;
	QVector<QColor> customColors;
	unsigned int customColorsSize;
	int scrollBarEnabled;

public:		// get Settings
	inline const unsigned int& getAutoplaySpeed() { return autoplaySpeed; }
	inline int getMaxAutoplay() { return (maxAutoplayEnabled) ? maxAutoplay : -1; }
	inline const unsigned int& getCellSize() { return cellSize; }
	inline const unsigned int& getGridWidth() { return gridWidth; }
	inline const unsigned int& getX() { return x; }
	inline const unsigned int& getY() { return y; }
	inline const unsigned int& getWindowStartmodus() { return windowStartmodus; }
	inline const unsigned int& getFixedWidth() { return fixedWidth; }
	inline const unsigned int& getFixedHeight() { return fixedHeight; }
	inline const bool& isFixed() { return fixedSize; }
	inline const bool& isMenuHidden() { return hiddenMenu; }
	inline const bool& isExitWarningEnabled() { return exitWarning; }
	inline const unsigned int& getTimerSize() { return timerSize; }
	inline const unsigned int& getGameNumber() { return gameNumber; }
	inline const QColor& getAliveCellColor() { return aliveCellColor; }
	inline const QColor& getDeadCellColor() { return deadCellColor; }
	inline const QColor& getLineColor() { return lineColor; }
	inline const QColor& getBackgroundColor() { return backgroundColor; }
	inline const int& isScrollBarEnabled() { return scrollBarEnabled; }
};

#endif // SETTINGSWINDOW_H
