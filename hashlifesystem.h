// © Copyright (c) 2018 SqYtCO

#ifndef HASHLIFESYSTEM_H
#define HASHLIFESYSTEM_H

#include "basesystem.h"
#include "hashlifemacrocell.h"
#include "hashlifetable.h"

// class implements HashLife algorithm
// algorithm supports only unlimitted fields
class HashLife_System : public Base_System
{
	Macrocell* master_cell;
	std::size_t level;

	static constexpr std::size_t init_size = 1024;

public:
	HashLife_System(std::size_t survival_rules, std::size_t rebirth_rules);

	virtual void random_cells(std::size_t, std::size_t) override {	}

	virtual void calc_next_generation() override { calc_next_generation(1); }
	virtual void next_generation() override { next_generation(1); }
	// generations has to be a power of 2
	std::size_t calc_next_generation(std::size_t generations);
	std::size_t next_generation(std::size_t generations);

	void resize(std::size_t size);

	inline bool cells_at_border()
	{
		// living cells at the border of inner macrocell (without empty border-macrocell)
		return (master_cell->nw->se->nw->population != 0 || master_cell->nw->se->ne->population != 0 || master_cell->nw->se->sw->population != 0 ||
				  master_cell->ne->sw->nw->population != 0 || master_cell->ne->sw->ne->population != 0 || master_cell->ne->sw->se->population != 0 ||
				  master_cell->se->nw->ne->population != 0 || master_cell->se->nw->se->population != 0 || master_cell->se->nw->sw->population != 0 ||
				  master_cell->sw->ne->nw->population != 0 || master_cell->sw->ne->se->population != 0 || master_cell->sw->ne->sw->population != 0);
	}

	inline void expand()
	{
		// double size of master_cell
		Macrocell* empty_temp = Macrocell::hash_table.get_empty(level - 1);
		master_cell = Macrocell::new_macrocell(Macrocell::new_macrocell(empty_temp, empty_temp, master_cell->nw, empty_temp),
												Macrocell::new_macrocell(empty_temp, empty_temp, empty_temp, master_cell->ne),
												Macrocell::new_macrocell(master_cell->se, empty_temp, empty_temp, empty_temp),
												Macrocell::new_macrocell(empty_temp, master_cell->sw, empty_temp, empty_temp));

		Macrocell::hash_table.set_level(++level);
		size_x *= 2;
		size_y *= 2;
	}

	inline void auto_expand()
	{
		// check if there are living cells at the border of inner macrocell (without empty border-macrocell)
		while(cells_at_border())
			expand();
	}

	std::size_t get_num_of_alive_cells() { return master_cell->population; }

// set functions
public:
	// set given cell to given state
	virtual void set_cell(std::size_t x, std::size_t y, Cell_State state) override
	{
		master_cell = master_cell->set_state(x + (size_x >> 1), y + (size_y >> 1), level, state);
	}

	// set all cells to given state
	inline virtual void set_all(Cell_State) override
	{
		master_cell = nullptr;
		resize(1024);
	}

// get functions
public:
	// return current cell state at given position
	virtual inline Cell_State get_cell_state(std::size_t x, std::size_t y) const override
	{
		return master_cell->get_state(x + (size_x >> 1), y + (size_y >> 1), level);
	}

	// return next cell state at given position; if the algorithm does not calculate next state, keep this implementation
	virtual inline Cell_State get_next_cell_state(std::size_t x, std::size_t y) const override
	{
		if(master_cell->result == nullptr)
			return Dead;

		return master_cell->result->get_state(x, y, level - 1);
	}
};

#endif // HASHLIFESYSTEM_H
