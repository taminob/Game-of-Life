// © Copyright (c) 2018 SqYtCO

#ifndef CELLSYSTEM_H
#define CELLSYSTEM_H

#include "basesystem.h"
#include "configuration.h"
#include <cstddef>
#include <vector>
#include <array>
#include <thread>

// class for one cell
struct Cell
{
	Cell_State state;
};

/* implementation of algorithm
 * in the algorithm two 1D container (std::vector for variable size) are used to store the cells with current and next state
 * each update is a toggle of the index (current_system) and the calculation of the next generation */
class Cell_System : public Base_System<std::vector<Cell>, 2>
{
	std::size_t current_system;					// index for currently active container

	Border_Behavior border_behavior;
	std::size_t live_rules;						// rules with how many neighbors a cell survives; standard: (1 << 2) | (1 << 3)=0xC
	std::size_t reborn_rules;					// rules with how many neighbors a cell is born; standard: (1 << 3)=0x8
	std::size_t num_of_threads;					// number of threads which are used for calculation
	std::vector<std::thread> threads;			// container for threads

	// return x on opposite side for neighbor cells beyond the grid
	inline std::size_t get_opposite_on_border_x(const long& x)
	{
		// leave grid on the right side -> go to left side
		if(x == static_cast<long>(size_x))
			return 0;
		// leave grid on the left side -> go to right side
		if(x == -1)
			return size_x - 1;

		// if grid is not left, return x
		return x;
	}

	// return y on opposite side for neighbor cells beyond the grid
	inline std::size_t get_opposite_on_border_y(const long& y)
	{
		// leave grid on the lower side -> go to the upper side
		if(y == static_cast<long>(size_y))
			return 0;
		// leave grid on the upper side -> go to the lower side
		if(y == -1)
			return size_y - 1;

		// if grid is not left, return y
		return y;
	}

	// return pos to cell with defined state beyond the grid
	inline std::size_t get_special_border(const long& x, const long& y)
	{
		// leave grid -> return pos to defined state
		if(x == -1 || x == static_cast<long>(size_x) || y == -1 || y == static_cast<long>(size_y))
			return size_y * size_x;

		// if grid is not left, return normal pos
		return y * size_x + x;
	}

	// calculate next states of all cells between max_x/max_y and min_x/min_y; used for calculation with multiple threads
	void calc_part(const std::size_t& max_x, const std::size_t& min_x, const std::size_t& max_y, const std::size_t& min_y);
	// calculate next state of one cell with "Continue_On_Opposite_Side" behavior; function pointers are too expensive because they can not be inlined
	void calc_cell_opposite_behavior(const std::size_t& x, const std::size_t& y);
	// calculate next state of one cell with "Surrounded_By_XXXX_Cells" behavior; function pointers are too expensive because they can not be inlined
	void calc_cell_special_border_behavior(const std::size_t& x, const std::size_t& y);

public:
	// init instance with given width, height, border behavior, rule sets and number of used threads; the given rule sets have to represent their rules with a bitmask
	Cell_System(const std::size_t& columns, const std::size_t& rows, const Border_Behavior& border_behavior,
				const std::size_t& live_rules = (1 << 2) | (1 << 3), const std::size_t& reborn_rules = (1 << 3), const std::size_t& threads = 1);
	virtual ~Cell_System() = default;

	virtual void random_cells(const std::size_t& alive, const std::size_t& dead) override;

	// calculate next state of every cell;
	virtual void calc_next_generation() override;
	virtual void next_generation() override;

// set functions
public:
	// calc_next_generation() must be called to calculate next state correctly
	virtual void set_cell(const std::size_t& x, const std::size_t& y, const Cell_State& state) override;
	// calc_next_generation() must not be called, next state is already set
	virtual void set_all(const Cell_State& state) override;

	// set number of threads for calculation
	void set_num_of_threads(const std::size_t& threads);

// get functions
public:
	// return current cell state at given position
	inline virtual const Cell_State& get_cell_state(const std::size_t& x, const std::size_t& y) const override
	{
		return system[current_system][y * size_x + x].state;
	}

	// return next cell state at given position
	inline virtual const Cell_State& get_next_cell_state(const std::size_t& x, const std::size_t& y) const override
	{
		return system[current_system ^ 0x01][y * size_x + x].state;
	}
};

#endif // CELLSYSTEM_H
