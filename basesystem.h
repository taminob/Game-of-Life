// © Copyright (c) 2018 SqYtCO

#ifndef BASESYSTEM_H
#define BASESYSTEM_H

#include <cstddef>		// std::size_t

// state of cell
enum Cell_State : char
{
	Dead = 0,
	Alive
};

/* algorithm interface
 * System_Container is considered as container for all cells
 * if the algorithm does not use a container, pass as second template parameter System_Count = 0 */
template<class System_Container, std::size_t System_Count>
class Base_System
{
protected:
	System_Container system[System_Count];		// (possibly multiple) container for cells

	std::size_t size_x;							// columns (width)
	std::size_t size_y;							// rows (height)

public:
	// initialize size_x and size_y with given width/height
	Base_System(const std::size_t& columns, const std::size_t& rows) : size_x(columns), size_y(rows) {	}
	virtual ~Base_System() = default;

	// generate random cells with given ratio
	virtual void random_cells(const std::size_t& alive, const std::size_t& dead) = 0;

	// calculate next generations without updating system; if the algorithm does not calculate next state, keep the empty implementation
	virtual void calc_next_generation() {	}
	// calculate next generation and update system
	virtual void next_generation() = 0;

// set functions
public:
	// set given cell to given state
	virtual void set_cell(const std::size_t& x, const std::size_t& y, const Cell_State& state) = 0;
	// set all cells to given state
	virtual void set_all(const Cell_State& state) = 0;

// get functions
public:
	// return current cell state at given position
	virtual const Cell_State& get_cell_state(const std::size_t& x, const std::size_t& y) const = 0;
	// return next cell state at given position; if the algorithm does not calculate next state, keep this implementation
	inline virtual const Cell_State& get_next_cell_state(const std::size_t& x, const std::size_t& y) const { return get_cell_state(x, y); }

	// return width
	inline virtual const std::size_t& get_size_x() const { return size_x; }
	// return height
	inline virtual const std::size_t& get_size_y() const { return size_y; }
};

#endif // BASESYSTEM_H
