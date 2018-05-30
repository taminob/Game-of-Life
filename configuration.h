// © Copyright (c) 2018 SqYtCO

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstddef>
#include <string>
#include <vector>

// behavior on border
enum Border_Behavior
{
	Continue_On_Opposite_Side = 0,
	Surrounded_By_Dead_Cells,
	Surrounded_By_Living_Cells
};

// values after reset
namespace Default_Values
{
	constexpr std::size_t SIZE_X = 100;
	constexpr std::size_t SIZE_Y = 100;
	constexpr std::size_t NUM_OF_THREADS = 4;
	constexpr std::size_t RELATION_DEAD = 2;
	constexpr std::size_t RELATION_ALIVE = 1;
	constexpr Border_Behavior BORDER_BEHAVIOR = Continue_On_Opposite_Side;
	constexpr const char* SAVE_PATH = "GOL_Saves/";
	constexpr std::size_t SAVE_NUM = 0;
	constexpr bool START_RANDOM = true;
	constexpr std::size_t LIVE_RULES = 0x0C;		// 2 & 3
	constexpr std::size_t REBORN_RULES = 0x08;		// 3
}

class Configuration
{
	// if all values of options are equal to saved values, "config_saved" is true; also true after reset
	bool config_saved;

protected:
	static constexpr const char* CONFIG_FILE = ".configuration";
	std::string config_path;

public:
	// try reading in config_file and set on failure default values
	Configuration();

	// return current save state of configuration
	const bool& get_saved() { return config_saved; }
	// config_path has to be empty or end on a separator; after setting config_path read in config
	void set_config_path(const std::string& new_config_path) { config_path = new_config_path; read_config(); }

	// set options to default value
	void reset_config();
	// read config_file and set options; return true on success and false on failure (file does not exist or no read permission)
	bool read_config();
	// write config_file; return true on success and false on failure (no write permission)
	bool write_config();

// options
private:
	std::size_t size_x;						// width (num of columns) of new created games
	std::size_t size_y;						// height (num of rows) of new created games
	std::size_t num_of_threads;				// number of used threads for calculation; the max. performance is reached when num_of_threads is equal to the number of cores
	std::size_t relation_dead;				// determines ratio between living and dead cells by creating random cells; relation_dead is the proportion of dead cells
	std::size_t relation_alive;				// determines ratio between living and dead cells by creating random cells; relation_alive is the proportion of living cells
	Border_Behavior border_behavior;		// set behavior on borders (neighbors of border cells)
	std::string save_path;					// path where saved games will be stored by default; the path must end with a separator
	bool start_random;						// if true, on startup only dead cells are generated
	std::size_t live_rules;					// ruleset to survive: numbers are represented by a bitmask: (1<<X); X=neighbors
	std::size_t reborn_rules;				// ruleset to be born: numbers are represented by a bitmask: (1<<X); X=neighbors

// set options
public:
	inline void set_size_x(const std::size_t& new_size_x) { config_saved = false; size_x = new_size_x; }
	inline void set_size_y(const std::size_t& new_size_y) { config_saved = false; size_y = new_size_y; }
	inline void set_num_of_threads(const std::size_t& new_num_of_threads) { config_saved = false; num_of_threads = new_num_of_threads; }
	inline void set_relation_dead(const std::size_t new_relation_dead) { config_saved = false; relation_dead = new_relation_dead; }
	inline void set_relation_alive(const std::size_t new_relation_alive) { config_saved = false; relation_alive = new_relation_alive; }
	inline void set_border_behavior(const Border_Behavior& new_border_behavior) { config_saved = false; border_behavior = new_border_behavior; }
	inline void set_save_path(const std::string& new_save_path) { config_saved = false; save_path = new_save_path; }
	inline void set_start_random(const bool& new_start_random) { config_saved = false; start_random = new_start_random; }
	inline void set_live_rules(const std::size_t& new_live_rules) { config_saved = false; live_rules = new_live_rules; }
	inline void set_reborn_rules(const std::size_t& new_reborn_rules) { config_saved = false; reborn_rules = new_reborn_rules; }

// get options
public:
	inline const std::size_t& get_size_x() const { return size_x; }
	inline const std::size_t& get_size_y() const { return size_y; }
	inline const std::size_t& get_num_of_threads() const { return num_of_threads; }
	inline const std::size_t& get_relation_dead() const { return relation_dead; }
	inline const std::size_t& get_relation_alive() const { return relation_alive; }
	inline const Border_Behavior& get_border_behavior() const { return border_behavior; }
	inline const std::string& get_save_path() const { return save_path; }
	inline const bool& get_start_random() const { return start_random; }
	inline const std::size_t& get_live_rules() { return live_rules; }
	inline const std::size_t& get_reborn_rules() { return reborn_rules; }
};

#endif // CONFIGURATION_H
