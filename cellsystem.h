#ifndef CELLSYSTEM_H
#define CELLSYSTEM_H

#include "cell.h"
#include <vector>

#define ALG1		// ALG1, ALG2, ALG3
// #define THREADS

class CellSystem : public std::vector< std::vector<Cell> >
{
	std::size_t generation;
	std::size_t maxX;
	std::size_t maxY;

#ifdef THREADS
	void task(int i, const int& v);
#endif

#ifdef ALG2
	int countNeighbors(const std::size_t& x, const std::size_t& y);
#elif defined ALG3
	bool check(int x, int y);
#endif

	void nextGenForCell(const std::size_t& x, const std::size_t& y);

public:
	CellSystem();

	void clear();

	void nextGen();
	void calcNextGen();

	void killAll();

	inline const std::size_t& getGeneration() const { return this->generation; }

	inline void setGeneration(const std::size_t& newGen) { generation = newGen; }
};

#endif // CELLSYSTEM_H
