// © Copyright (c) 2018 SqYtCO

#ifndef CORE_H
#define CORE_H

#include "configuration.h"
#include "basesystem.h"
#include <memory>

// singleton class: to get the instance call Core::get_instance()
// this core contains all non-graphic and basic operations and manage cells
// the class is completely written in standard C++ (filesystem support required) and can also be used in a GUI-less application
class Core
{
	// contain all non-graphic configurations
	static Configuration config;

	static std::unique_ptr<Base_System> system_;
	static std::size_t generation;

public:
	// create new game with set configuration
	// if Configuration::start_random is true, the game board will be filled with random cells; the ratio is given by Configuration::relation_dead/Configuration::relation_alive
	static void new_system();

	// save current game
	// if "name" is empty, a name is generated from date and time and written to Configuration::save_path
	// "name" has to contain an absolute or relative path (from exec dir) to file; in this case Configuration::save_path is not used!
	static bool save(std::string file = std::string());
	// load given file; the file must contain a valid format (0=dead, 1=alive, \n=next_row, space=ignored); file must end on ".gol", otherwise false will be returned
	static bool load(const std::string& file);

	// update to next generation and calculate next states
	static std::size_t next_generation(std::size_t generations = 1);
	// calculate next states without updating cells
	static void calc_next_generation(std::size_t generations = 1);

	static std::size_t get_num_of_alive_cells();
	static bool expand();

// set member
public:
	// set one cell at given position to given state; the next state is not calculated; to do this call calc_next_generation()
	static inline void set_cell_state(std::size_t x, std::size_t y, Cell_State state)
	{
		system_->set_cell(x, y, state);
	}
	// set all cells to given state; the next state is already calculated, no call of calc_next_generation() required
	static inline void reset_cells(Cell_State state = Dead)
	{
		system_->set_all(state); generation = 0;
	}
// get member
public:
	static inline Configuration* get_config()
	{
		return &config;
	}
	static inline Cell_State get_cell_state(std::size_t x, std::size_t y)
	{
		return system_->get_cell_state(x, y);
	}
	static inline Cell_State get_next_cell_state(std::size_t x, std::size_t y)
	{
		return system_->get_next_cell_state(x, y);
	}
	static inline std::size_t get_size_x()
	{
		return system_->get_size_x();
	}
	static inline std::size_t get_size_y()
	{
		return system_->get_size_y();
	}
	static inline std::size_t get_generation()
	{
		return generation;
	}
};

#endif // CORE_H
