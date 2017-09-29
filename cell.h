// © Copyright (c) 2017 SqYtCO

#ifndef CELL_H
#define CELL_H

struct Cell
{
	Cell(const bool& alive) : alive(alive), next(alive) {	}

	bool alive : 1;			// current state; size: 1 bit
	bool next : 1;			// next state; size: 1 bit
};

#endif // CELL_H
