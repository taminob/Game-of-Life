// © Copyright (c) 2018 SqYtCO

#ifndef CORE_H
#define CORE_H

#include "configuration.h"
#include "cellsystem.h"

// singleton class: to get the instance call Core::get_instance()
// this core contains all non-graphic and basic operations and manage cells
// the class is completely written in standard C++ (filesystem support required) and can also be used in a GUI-less application
class Core
{
	// contain all non-graphic configurations
	Configuration config;

	// will contain
	// the template parameter have to be exchanged if another implementation is used
	Cell_System* cells;
	std::size_t generation;

	// private constructors and assignment operator
	Core();
	Core(const Core&);
	Core& operator=(const Core&);

public:
	// singleton pattern - only one core per application; get this single instance by calling Core::get_instance()
	static Core* get_instance();
	// public dtor; delete cells
	~Core();

	// save current game
	// if "name" is empty, a name is generated from date and time and written to Configuration::save_path
	// "name" has to contain an absolute or relative path (from exec dir) to file; in this case Configuration::save_path is not used!
	bool save(std::string file = std::string());
	// load given file; the file must contain a valid format (0=dead, 1=alive, \n=next_row, space=ignored); file must end on ".gol", otherwise false will be returned
	bool load(const std::string& file);

	// create new game with set configuration
	// if Configuration::start_random is true, the game board will be filled with random cells; the ratio is given by Configuration::relation_dead/Configuration::relation_alive
	void new_system();
	// update to next generation and calculate next states
	const std::size_t& next_generation();
	// calculate next states without updating cells
	void calc_next_generation();

// set member
public:
	// set one cell at given position to given state; the next state is not calculated; to do this call calc_next_generation()
	inline void set_cell_state(const std::size_t& x, const std::size_t& y, const Cell_State& state) const { cells->set_cell(x, y, state); }
	// set all cells to given state; the next state is already calculated, no call of calc_next_generation() required
	inline void reset_cells(const Cell_State& state = Cell_State::Dead) { cells->set_all(state); generation = 0; }

// get member
public:
	inline Configuration* get_config() { return &config; }
	inline const Cell_State& get_cell_state(const std::size_t& x, const std::size_t& y) const { return cells->get_cell_state(x, y); }
	inline const Cell_State& get_next_cell_state(const std::size_t& x, const std::size_t& y) const { return cells->get_next_cell_state(x, y); }
	inline const std::size_t& get_size_x() const { return cells->get_size_x(); }
	inline const std::size_t& get_size_y() const { return cells->get_size_y(); }
	inline const std::size_t& get_generation() const { return generation; }
};

#endif // CORE_H
