#ifndef HASHLIFEMACROCELL_H
#define HASHLIFEMACROCELL_H

#include "basesystem.h"
#include "hashlifetable.h"
#include <map>
#include <vector>
#include <cmath>
#include <array>
#include <unordered_map>

struct Macrocell
{
	static HashLife_Table hash_table;

	std::size_t population;

	Macrocell* nw;
	Macrocell* ne;
	Macrocell* se;
	Macrocell* sw;
	Macrocell* result;

	static Macrocell* new_macrocell(Macrocell* nw, Macrocell* ne, Macrocell* se, Macrocell* sw);

	Macrocell(Macrocell* nw, Macrocell* ne, Macrocell* se, Macrocell* sw);

	inline std::size_t hash() const
	{
		return ::hash(nw, ne, se, sw);
	}

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

	Macrocell* set_state(std::size_t x, std::size_t y, std::size_t level, Cell_State state);
	inline bool get_state(std::size_t x, std::size_t y, std::size_t level) const
	{
		if(population == 0)
			return false;
		if(level == 0)
			return population;

		std::size_t half = 0x01 << (--level);
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

	// generations has to be of 2^x
	Macrocell* calculate(std::size_t level, std::size_t generations = 1);

	inline Macrocell* center() const
	{
	   return new_macrocell(nw->se, ne->sw, se->nw, sw->ne);
	}
	inline Macrocell* nn() const
	{
		return new_macrocell(nw->ne, ne->nw, ne->sw, nw->se);
	}
	inline Macrocell* ee() const
	{
		return new_macrocell(ne->sw, ne->se, se->ne, se->nw);
	}
	inline Macrocell* ss() const
	{
		return new_macrocell(sw->ne, se->nw, se->sw, sw->se);
	}
	inline Macrocell* ww() const
	{
		return new_macrocell(nw->sw, nw->se, sw->ne, sw->nw);
	}
};

#endif // HASHLIFEMACROCELL_H
