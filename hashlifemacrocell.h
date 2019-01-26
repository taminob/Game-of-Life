// © Copyright (c) 2018 SqYtCO

#ifndef HASHLIFEMACROCELL_H
#define HASHLIFEMACROCELL_H

#include "basesystem.h"

class HashLife_Table;

// class for recursive hashlife algorithm
struct Macrocell
{
	static HashLife_Table hash_table;

	// num of living zero level cells in macrocell
	std::size_t population;

	// smaller daughter cells with level n-1
	Macrocell* nw;
	Macrocell* ne;
	Macrocell* se;
	Macrocell* sw;
	// next state of macrocell
	Macrocell* result;

	// return macrocell with given daughter cells; if cell does not exist in hash_table, a new cell is created and inserted into the hash_table
	static Macrocell* new_macrocell(Macrocell* nw, Macrocell* ne, Macrocell* se, Macrocell* sw);

	// init instance; if ne == nullptr, the cell will be a zero level cell; nw != nullptr sets the population of a zero level cell to 1, otherwise it is 0
	Macrocell(Macrocell* nw, Macrocell* ne, Macrocell* se, Macrocell* sw);

	// return the hashed value of the daughter cells
	inline std::size_t hash() const;

	// calculate level of cell
	std::size_t level() const
	{
		const Macrocell* temp = this;
		std::size_t level = 0;
		while(temp)
		{
			++level;
			temp = temp->ne;
		}

		return level;
	}

	// set zero level cell at x,y to given state; upper left corner is 0,0
	Macrocell* set_state(std::size_t x, std::size_t y, std::size_t level, Cell_State state);
	// get zero level cell state at x,y; upper left corner is 0,0
	inline bool get_state(std::size_t x, std::size_t y, std::size_t level) const
	{
		if(population == 0)
			return false;
		if(level == 0)
			return population;

		std::size_t half = 0x01ull << (--level);
		if(x < half)
		{
			if(y < half)
				return nw->get_state(x, y, level);
			else
				return sw->get_state(x, y - half, level);
		}
		else
		{
			if(y < half)
				return ne->get_state(x - half, y, level);
			else
				return se->get_state(x - half, y - half, level);
		}
	}

	// generations has to be a power of two
	Macrocell* calculate(std::size_t level, std::size_t generations = 1);

	// get macrocell of level n-1 which is located in the center
	inline Macrocell* center() const
	{
	   return new_macrocell(nw->se, ne->sw, se->nw, sw->ne);
	}
	// get macrocell of level n-1 which is located in the north
	inline Macrocell* nn() const
	{
		return new_macrocell(nw->ne, ne->nw, ne->sw, nw->se);
	}
	// get macrocell of level n-1 which is located in the east
	inline Macrocell* ee() const
	{
		return new_macrocell(ne->sw, ne->se, se->ne, se->nw);
	}
	// get macrocell of level n-1 which is located in the south
	inline Macrocell* ss() const
	{
		return new_macrocell(sw->ne, se->nw, se->sw, sw->se);
	}
	// get macrocell of level n-1 which is located in the west
	inline Macrocell* ww() const
	{
		return new_macrocell(nw->sw, nw->se, sw->ne, sw->nw);
	}
};

#endif // HASHLIFEMACROCELL_H
