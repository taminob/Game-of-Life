// © Copyright (c) 2018 SqYtCO

#include "hashlifemacrocell.h"
#include "hashlifetable.h"
#include <algorithm>
#include <cassert>

HashLife_Table Macrocell::hash_table;

Macrocell* Macrocell::new_macrocell(Macrocell* nw, Macrocell* ne, Macrocell* se, Macrocell* sw)
{
	Macrocell* temp = Macrocell::hash_table.get(nw, ne, se, sw);
	if(temp)
		return temp;
	else
	{
		temp = new Macrocell(nw, ne, se, sw);
		Macrocell::hash_table.insert(temp, temp);
		return temp;
	}
}

Macrocell::Macrocell(Macrocell* nw, Macrocell* ne, Macrocell* se, Macrocell* sw)
	: nw(nw), ne(ne), se(se), sw(sw), result(nullptr)
{
	if(ne == nullptr)
		this->population = (nw == reinterpret_cast<Macrocell*>(0x01));
	else
		this->population = this->nw->population + this->ne->population + this->se->population + this->sw->population;
}

std::size_t Macrocell::hash() const
{
	return HashLife_Table::hash(nw, ne, se, sw);
}

Macrocell* Macrocell::set_state(std::size_t x, std::size_t y, std::size_t level, Cell_State state)
{
	if(ne == nullptr)
		return (state) ? hash_table.get_alive() : hash_table.get_dead();
	if(!state && population == 0)
		return this;

	Macrocell* cell = new Macrocell(nw, ne, se, sw);
	std::size_t size = 0x01ull << (--level);

	if(x < size)
	{
		if(y < size)
			cell->nw = nw->set_state(x, y, level, state);
		else
			cell->sw = sw->set_state(x, y - size, level, state);
	}
	else
	{
		if(y < size)
			cell->ne = ne->set_state(x - size, y, level, state);
		else
			cell->se = se->set_state(x - size, y - size, level, state);
	}

	Macrocell* temp = hash_table[cell];
	if(temp)
	{
		delete cell;
		return temp;
	}
	else
	{
		if(state)
			cell->population = population + 1;
		else
			cell->population = population - 1;

		hash_table.insert(cell, cell);
	}

	return cell;
}

Macrocell* Macrocell::calculate(std::size_t level, std::size_t generations)
{
	if(result)
		return result;
	if(population == 0)
	{
		result = Macrocell::hash_table.get_empty()[level - 1];
		return result;
	}

	std::size_t half = level - 3;
	Macrocell* nw_ = nw->calculate(level - 1, generations);
	Macrocell* nn_ = nn()->calculate(level - 1, generations);
	Macrocell* ne_ = ne->calculate(level - 1, generations);
	Macrocell* ww_ = ww()->calculate(level - 1, generations);
	Macrocell* cc_ = center()->calculate(level - 1, generations);
	Macrocell* ee_ = ee()->calculate(level - 1, generations);
	Macrocell* sw_ = sw->calculate(level - 1, generations);
	Macrocell* ss_ = ss()->calculate(level - 1, generations);
	Macrocell* se_ = se->calculate(level - 1, generations);

	if(generations > half)
	{
		nw_ = Macrocell::new_macrocell(nw_, nn_, cc_, ww_)->calculate(level - 1, generations);
		ne_ = Macrocell::new_macrocell(nn_, ne_, ee_, cc_)->calculate(level - 1, generations);
		se_ = Macrocell::new_macrocell(cc_, ee_, se_, ss_)->calculate(level - 1, generations);
		sw_ = Macrocell::new_macrocell(ww_, cc_, ss_, sw_)->calculate(level - 1, generations);
	}
	else
	{
		nw_ = Macrocell::new_macrocell(nw_->se, nn_->sw, cc_->nw, ww_->ne);
		ne_ = Macrocell::new_macrocell(nn_->se, ne_->sw, ee_->nw, cc_->ne);
		se_ = Macrocell::new_macrocell(cc_->se, ee_->sw, se_->nw, ss_->ne);
		sw_ = Macrocell::new_macrocell(ww_->se, cc_->sw, ss_->nw, sw_->ne);
	}

	result = Macrocell::new_macrocell(nw_, ne_, se_, sw_);
	return result;
}
