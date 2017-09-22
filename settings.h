#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVector>
#include <QSettings>
#include <QColor>

class SettingsWindow;

class Settings : public QObject
{
	Q_OBJECT

	friend class SettingsWindow;

	SettingsWindow* settwin;
	QSettings* settings;
	bool saved;

	QPalette* aliveCellColorPalette;
	QPalette* deadCellColorPalette;
	QPalette* lineColorPalette;
	QPalette* nextGenAliveCellColorPalette;
	QPalette* nextGenDeadCellColorPalette;
	QPalette* backgroundColorPalette;

public:
	explicit Settings(SettingsWindow* settwin, QObject* parent = nullptr);

	QSettings* setting() { return settings; }

	enum Language { English, German };

private:
// Settings
	unsigned int autoplaySpeed;				// new attributs need:
	bool maxAutoplayEnabled;				// - attribute
	int maxAutoplay;						// - get-Member
	unsigned int cellSize;					// - slot
	unsigned int gridWidth;					// - default-Value
	unsigned int x;							// - write Settings
	unsigned int y;							// - load Settings
	unsigned int windowStartmodus;			//  SettingsWindow:
	bool fixedSize;							// - update Settings
	unsigned int fixedWidth;				// - private Member: Check-/Spin-/Combobox
	unsigned int fixedHeight;
	bool hiddenMenu;
	bool exitWarning;
	bool dynSettingChange;
	unsigned int timerSize;
	unsigned int gameNumber;
	QColor aliveCellColor;
	QColor deadCellColor;
	QColor lineColor;
	QColor nextGenAliveCellColor;
	QColor nextGenDeadCellColor;
	QColor backgroundColor;
	QVector<QColor> customColors;
	unsigned int customColorsSize;
	int scrollBarEnabled;
	Language language;			// 0: English; 1: Deutsch;

public:
// get Settings
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
	inline const QColor& getNextGenAliveCellColor() { return nextGenAliveCellColor; }
	inline const QColor& getNextGenDeadCellColor() { return nextGenDeadCellColor; }
	inline const QColor& getBackgroundColor() { return backgroundColor; }
	inline const int& isScrollBarEnabled() { return scrollBarEnabled; }
	inline const Language& getLanguage() { return language; }

public slots:
	void setDefaultValues();
	void saveSettings();
	void loadSettings();

	void setGameNumber(int newValue);
	void setLanguage(int newValue);

private slots:
	void setNewAutoplaySpeed(int newValue);
	void maxAutoplayActivated(bool activated);
	void setNewMaxAutoplay(int newValue);
	void setNewWindowStartmodus(int newValue);
	void fixedSizeActivated(bool activated);
	void setNewFixedWidth(int newValue);
	void setNewFixedHeight(int newValue);
	void hideMenuActivated(bool activated);
	void exitWarningActivated(bool activated);
	void setNewCellSize(int newValue);
	void setNewGridWidth(int newValue);
	void setNewX(int newValue);
	void setNewY(int newValue);
	void setNewTimerSize(int newValue);
	void newAliveCellColor();
	void newDeadCellColor();
	void newLineColor();
	void newNextGenAliveCellColor();
	void newNextGenDeadCellColor();
	void newBackgroundColor();
	void dynSettingChangeEnabled(bool activated);
	void newScrollBarPolicy(int newValue);

signals:
	void autoplaySpeedChanged();
	void languageChanged(Language);
};

#endif // SETTINGS_H
