#ifndef CELL_H
#define CELL_H

struct Cell
{
	Cell(const bool& alive) : alive(alive), next(alive) {	}

	bool alive;
	bool next;
};

#endif // CELL_H
