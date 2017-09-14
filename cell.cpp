#include "cell.h"
#include <vector>

Cell::Cell(std::vector< std::vector<Cell> > &system, bool alive, int x, int y) : system(system), x(y), y(x), maxX(system[0].size() - 1), maxY(system.size() - 1)
{
	this->alive = alive;
	this->generation = 0;
}

Cell::Cell(std::vector<std::vector<Cell> > &system, bool alive, int x, int y, const int MAX_X, const int MAX_Y) : system(system), x(y), y(x), maxX(MAX_Y), maxY(MAX_X)
{
	this->alive = alive;
	this->generation = 0;
}

Cell::Cell(const Cell& copy) : system(copy.system), x(copy.x), y(copy.y), maxX(copy.maxX), maxY(copy.maxY)
{
	this->alive = copy.alive;
	this->generation = copy.generation;
}

Cell &Cell::operator =(const Cell &oth)
{
	this->system = oth.system;
	this->x = oth.x;
	this->y = oth.y;
	this->maxX = oth.maxX;
	this->maxY = oth.maxY;
	this->generation = oth.generation;

	return *this;
}

void Cell::nextOne()
{
	int aliveCellNeighbs = 0;

	if(x == 0)
	{
		if(y == 0)
		{
			if(system[x][y + 1].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y + 1].alive)
				++aliveCellNeighbs;
		}
		else if(y == maxY)
		{
			if(system[x][y - 1].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y - 1].alive)
				++aliveCellNeighbs;
		}
		else
		{
			if(system[x][y - 1].alive)
				++aliveCellNeighbs;
			if(system[x][y + 1].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y - 1].alive)
				++aliveCellNeighbs;
			if(system[x + 1][y + 1].alive)
				++aliveCellNeighbs;
		}


	}
	else if(x == maxX)
	{
		if(y == 0)
		{
			if(system[x][y + 1].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y + 1].alive)
				++aliveCellNeighbs;
		}
		else if(y == maxY)
		{
			if(system[x][y - 1].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y - 1].alive)
				++aliveCellNeighbs;
		}
		else
		{
			if(system[x][y - 1].alive)
				++aliveCellNeighbs;
			if(system[x][y + 1].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y - 1].alive)
				++aliveCellNeighbs;
			if(system[x - 1][y + 1].alive)
				++aliveCellNeighbs;
		}

	}
	else if(y == 0)
	{
		if(system[x][y + 1].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y + 1].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y + 1].alive)
			++aliveCellNeighbs;
	}
	else if(y == maxY)
	{
		if(system[x][y - 1].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y - 1].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y - 1].alive)
			++aliveCellNeighbs;
	}
	else
	{
		if(system[x][y - 1].alive)
			++aliveCellNeighbs;
		if(system[x][y + 1].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y - 1].alive)
			++aliveCellNeighbs;
		if(system[x - 1][y + 1].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y - 1].alive)
			++aliveCellNeighbs;
		if(system[x + 1][y + 1].alive)
			++aliveCellNeighbs;
	}

	if(aliveCellNeighbs > 3)
	{
		system[x][y].next = false;
	}
	else if(aliveCellNeighbs < 2)
	{
		system[x][y].next = false;
	}
	else if(aliveCellNeighbs == 3)
	{
		system[x][y].next = true;
	}
	else if(aliveCellNeighbs == 2 && system[x][y].alive)
	{
		system[x][y].next = true;
	}
	else
	{
		system[x][y].next = false;
	}
}

std::vector<std::vector<Cell> > &Cell::nextGen()
{
	for(auto &a : system)
	{
		for(auto &b : a)
		{
			b.nextOne();
		}
	}

	for(auto &a : system)
	{
		for(auto &b : a)
		{
			b.alive = b.next;
			++b.generation;
		}
	}

	return system;
}
