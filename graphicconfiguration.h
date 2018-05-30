// © Copyright (c) 2018 SqYtCO

#ifndef GRAPHICCONFIGURATION_H
#define GRAPHICCONFIGURATION_H

#include <QColor>
#include <cstddef>

// display language
enum Language
{
	System = 0,
	English,
	German
};

// values after reset
namespace Default_Values
{
	constexpr Language LANGUAGE = System;
	constexpr std::size_t CELL_SIZE = 20;
	constexpr bool GRID_ACTIVE = true;
	constexpr std::size_t GENERATION_COUNTER_SIZE = 40;
	constexpr bool EXIT_WARNING = true;
	const QColor DEAD_COLOR = QColor(0x00, 0x00, 0x00);
	const QColor ALIVE_COLOR = QColor(0x00, 0xFF, 0x00);
	const QColor REVIVING_COLOR = QColor(0x22, 0x33, 0x22);
	const QColor DYING_COLOR = QColor(0x80, 0xFF, 0x80);
	const QColor BACKGROUND_COLOR = QColor(0x80, 0x80, 0x80);
	constexpr bool HIDE_GENERATION_COUNTER = false;
	constexpr bool FULLSCREEN = false;
	constexpr bool LOCK_AFTER_FIRST_GENERATING = false;
	constexpr bool LEFT_ALIVE_AND_RIGHT_DEAD = true;
	constexpr std::size_t GENERATIONS_PER_STEP = 1;
	constexpr bool SHOW_STARTUP_DIALOG = true;
	constexpr std::size_t DELAY = 15;
}

class GraphicConfiguration
{
	// if all values of options are equal to saved values, "graphic_config_saved" is true; also true after reset
	bool graphic_config_saved;

protected:
	constexpr static const char* GCONFIG_FILE = ".gconfiguration";
	std::string gconfig_path;

public:
	// try reading in config_file and set on failure default values
	GraphicConfiguration();

	// return current save state of graphic configuration
	const bool& get_saved() { return graphic_config_saved; }
	// gconfig_path has to be empty or end on a separator; after setting gconfig_path read in config
	void set_config_path(const std::string& new_config_path) { gconfig_path = new_config_path; read_config(); }

	// set options to default value
	void reset_config();
	// read config_file and set options; return true on success and false on failure (file does not exist or no read permission)
	bool read_config();
	// write config_file; return true on success and false on failure (no write permission)
	bool write_config();

// options
private:
	Language language;
	std::size_t cell_size;					// startsize of drawn cells in pixels
	bool grid_active;						// if true, a grey grid is drawn around the cells
	std::size_t generation_counter_size;	// size of generation counter in pixels
	bool exit_warning;						// if true, a message is shown on close to save or discard the game
	QColor dead_color;						// color of dead cells
	QColor alive_color;						// color of alive cells
	QColor reviving_color;					// color of reviving (dead but in next generation alive) cells
	QColor dying_color;						// color of dying (alive but in next generation dead) cells
	QColor background_color;				// color of background outside of the game grid
	bool hide_generation_counter;			// if true, no generation timer is shown
	bool fullscreen;						// if true, the window will start in fullscreen mode
	bool lock_after_first_generating;		// if true, it is impossible to change cells after initial generation
	bool left_alive_and_right_dead;			// if true, the left mouse button makes cells alive and the right mouse button makes cells dead; if false, the behavior is inverted
	std::size_t generations_per_step;		// generations per step; if >1 some generations will calculated but not displayed
	bool show_startup_dialog;				// if true, a startup dialog is shown; config will be written automatically after setting
	std::size_t delay;						// delay between generations while autogenerating is running

// set options
public:
	inline void set_language(const Language& new_language) { graphic_config_saved = false; language = new_language; }
	inline void set_cell_size(const std::size_t& new_cell_size) { graphic_config_saved = false; cell_size = new_cell_size; }
	inline void set_grid_active(const bool& new_grid_active) { graphic_config_saved = false; grid_active = new_grid_active; }
	inline void set_generation_counter_size(const std::size_t& new_generation_counter_size) { graphic_config_saved = false; generation_counter_size = new_generation_counter_size; }
	inline void set_exit_warning(const bool& new_exit_warning) { graphic_config_saved = false; exit_warning = new_exit_warning; }
	inline void set_dead_color(const QColor& new_dead_color) { graphic_config_saved = false; dead_color = new_dead_color; }
	inline void set_alive_color(const QColor& new_alive_color) { graphic_config_saved = false; alive_color = new_alive_color; }
	inline void set_reviving_color(const QColor& new_reviving_color) { graphic_config_saved = false; reviving_color = new_reviving_color; }
	inline void set_dying_color(const QColor& new_dying_color) { graphic_config_saved = false; dying_color = new_dying_color; }
	inline void set_background_color(const QColor& new_background_color) { graphic_config_saved = false; background_color = new_background_color; }
	inline void set_hide_generation_counter(const bool& new_hide_generation_counter) { graphic_config_saved = false; hide_generation_counter = new_hide_generation_counter; }
	inline void set_fullscreen(const bool& new_fullscreen) { graphic_config_saved = false; fullscreen = new_fullscreen; }
	inline void set_lock_after_first_generating(const bool& new_lock_after_first_generating) { graphic_config_saved = false; lock_after_first_generating = new_lock_after_first_generating; }
	inline void set_left_alive_and_right_dead(const bool& new_left_alive_and_right_dead) { graphic_config_saved = false; left_alive_and_right_dead = new_left_alive_and_right_dead; }
	inline void set_generations_per_step(const std::size_t& new_generations_per_step) { graphic_config_saved = false; generations_per_step = new_generations_per_step; }
	inline void set_show_startup_dialog(const bool& new_show_startup_dialog) { show_startup_dialog = new_show_startup_dialog; write_config(); }
	inline void set_delay(const std::size_t& new_delay) { graphic_config_saved = false; delay = new_delay; }

// get options
public:
	inline const Language& get_language() const { return language; }
	inline const std::size_t& get_cell_size() const { return cell_size; }
	inline const bool& get_grid_active() const { return grid_active; }
	inline const std::size_t& get_generation_counter_size() const { return generation_counter_size; }
	inline const bool& get_exit_warning() const { return exit_warning; }
	inline const QColor& get_dead_color() const { return dead_color; }
	inline const QColor& get_alive_color() const { return alive_color; }
	inline const QColor& get_reviving_color() const { return reviving_color; }
	inline const QColor& get_dying_color() const { return dying_color; }
	inline const QColor& get_background_color() const { return background_color; }
	inline const bool& get_hide_generation_counter() const { return hide_generation_counter; }
	inline const bool& get_fullscreen() const { return fullscreen; }
	inline const bool& get_lock_after_first_generating() const { return lock_after_first_generating; }
	inline const bool& get_left_alive_and_right_dead() const { return left_alive_and_right_dead; }
	inline const std::size_t& get_generations_per_step() const { return generations_per_step; }
	inline const bool& get_show_startup_dialog() const { return show_startup_dialog; }
	inline const std::size_t& get_delay() const { return delay; }
};

#endif // GRAPHICCONFIGURATION_H
