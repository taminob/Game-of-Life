#ifndef CELLSYSTEM_H
#define CELLSYSTEM_H

#include <vector>

#define ALG1		// ALG1, ALG2, ALG3
// #define THREADS

struct Cell;

class CellSystem : public std::vector< std::vector<Cell> >
{
	unsigned int generation;
	unsigned int maxX;
	unsigned int maxY;
#ifdef ALG2
	int countNeighbors(const unsigned int& x, const unsigned int& y);
#elif defined ALG3
	bool check(int x, int y);
#endif

	void nextGenForCell(const unsigned int& x, const unsigned int& y);

public:
	CellSystem();

	void clear();

	void nextGen();
	void calcNextGen();

	void killAll();

	inline const unsigned int& getGeneration() const { return this->generation; }

	inline void setGeneration(const unsigned int& newGen) { generation = newGen; }
};

#endif // CELLSYSTEM_H
