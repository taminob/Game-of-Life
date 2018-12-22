// © Copyright (c) 2018 SqYtCO

#ifndef CELLSYSTEM_H
#define CELLSYSTEM_H

#include "basesystem.h"
#include "configuration.h"
#include <cstddef>
#include <vector>
#include <array>
#include <thread>

/* implementation of algorithm
 * in the algorithm two 1D container (std::vector for variable size) are used to store the cells with current and next state
 * each update is a toggle of the index (current_system) and the calculation of the next generation */
class Cell_System : public Base_System
{
	std::vector<Cell_State> system[2];				// containers for cells

	std::size_t current_system;					// index for currently active container

	std::size_t survival_rules;						// rules with how many neighbors a cell survives; standard: (1 << 2) | (1 << 3)=0xC
	std::size_t rebirth_rules;					// rules with how many neighbors a cell is born; standard: (1 << 3)=0x8
	std::size_t num_of_threads;					// number of threads which are used for calculation
	std::vector<std::thread> threads;			// container for threads

	Border_Behavior border_behavior;

	// return x on opposite side for neighbor cells beyond the grid
	inline std::size_t get_opposite_on_border_x(long x)
	{
		// leave grid on the right side -> go to left side
		if(x >= static_cast<long>(size_x))
			return 0;
		// leave grid on the left side -> go to right side
		if(x < 0)
			return size_x - 1;

		// if grid is not left, return x
		return static_cast<std::size_t>(x);
	}

	// return y on opposite side for neighbor cells beyond the grid
	inline std::size_t get_opposite_on_border_y(long y)
	{
		// leave grid on the lower side -> go to the upper side
		if(y >= static_cast<long>(size_y))
			return 0;
		// leave grid on the upper side -> go to the lower side
		if(y < 0)
			return size_y - 1;

		// if grid is not left, return y
		return static_cast<std::size_t>(y);
	}

	// return pos to cell with defined state beyond the grid
	inline std::size_t get_special_border(long x, long y)
	{
		// leave grid -> return pos to defined state
		if(x < 0 || x >= static_cast<long>(size_x) || y < 0 || y >= static_cast<long>(size_y))
			return size_y * size_x;

		// if grid is not left, return normal pos
		return static_cast<std::size_t>(y) * size_x + static_cast<std::size_t>(x);
	}

	// calculate next states of all cells between max_x/max_y and min_x/min_y; used for calculation with multiple threads
	void calc_part(std::size_t max_x, std::size_t min_x, std::size_t max_y, std::size_t min_y);
	// calculate next state of one cell with "Continue_On_Opposite_Side" behavior; function pointers are too expensive because they can not be inlined
	void calc_cell_opposite_behavior(std::size_t x, std::size_t y);
	// calculate next state of one cell with "Surrounded_By_XXXX_Cells" behavior; function pointers are too expensive because they can not be inlined
	void calc_cell_special_border_behavior(std::size_t x, std::size_t y);

public:
	// init instance with given width, height, border behavior, rule sets and number of used threads; the given rule sets have to represent their rules with a bitmask
	Cell_System(std::size_t columns, std::size_t rows, const Border_Behavior& border_behavior,
				std::size_t survival_rules = (1 << 2) | (1 << 3), std::size_t rebirth_rules = (1 << 3), std::size_t threads = 1);
	virtual ~Cell_System() override = default;

	virtual void random_cells(std::size_t alive, std::size_t dead) override;

	// calculate next state of every cell;
	virtual void calc_next_generation() override;
	virtual void next_generation() override;

// set functions
public:
	// calc_next_generation() must be called to calculate next state correctly
	virtual void set_cell(std::size_t x, std::size_t y, Cell_State state) override;
	// calc_next_generation() must not be called, next state is already set
	virtual void set_all(Cell_State state) override;

	// set number of threads for calculation
	void set_num_of_threads(std::size_t threads);

// get functions
public:
	// return current cell state at given position
	inline virtual Cell_State get_cell_state(std::size_t x, std::size_t y) const override
	{
		return system[current_system][y * size_x + x];
	}

	// return next cell state at given position
	inline virtual Cell_State get_next_cell_state(std::size_t x, std::size_t y) const override
	{
		return system[current_system ^ 0x01][y * size_x + x];
	}
};

#endif // CELLSYSTEM_H
