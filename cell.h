#ifndef CELL_H
#define CELL_H

#include <vector>

class Cell
{
	bool alive;
	bool next;

	unsigned int generation;

	void nextOne();

	std::vector<std::vector<Cell> > &system;

	int x;
	int y;

	int maxX;
	int maxY;

public:
	Cell(std::vector<std::vector<Cell> > &system, bool alive, int x, int y);
	Cell(std::vector<std::vector<Cell> > &system, bool alive, int x, int y, const int MAX_X, const int MAX_Y);

	Cell(const Cell &copy);
	Cell& operator =(const Cell& oth);

	std::vector<std::vector<Cell> > &nextGen();

	inline const unsigned int& getGeneration() { return this->generation; }

	inline const bool& isAlive() const { return this->alive; }

	inline const int& getX() { return x; }
	inline const int& getY() { return y; }

	inline bool& getOriginalAlive() { return alive; }
	inline unsigned int& getOriginalGeneration() { return this->generation; }
};

#endif // CELL_H
