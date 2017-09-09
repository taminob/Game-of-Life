#include "cellsystem.h"
#include "cell.h"
#include <atomic>
#include <thread>
#include <mutex>

CellSystem::CellSystem() : std::vector< std::vector<Cell> >()
{
	generation = 0;
	maxX = 0;
	maxY = 0;
}

void CellSystem::clear()
{
	for(unsigned int i = 0; i < this->size(); ++i)
	{
		(*this)[i].clear();
	}

	std::vector< std::vector<Cell> >::clear();
	maxX = 0;
	maxY = 0;
}

void CellSystem::nextGen()
{
	for(unsigned int a = 0; a <= maxX; ++a)
	{
		for(unsigned int b = 0; b <= maxY; ++b)
		{
			(*this)[a][b].alive = (*this)[a][b].next;
		}
	}

	++generation;

#ifndef THREADS
	for(unsigned int a = 0; a <= maxX; ++a)
	{
		for(unsigned int b = 0; b <= maxY; ++b)
		{
			nextGenForCell(a, b);
		}
	}
#else
	const auto task = [&](const int& i)
	{
		for(int a = i; a <= maxY; ++a)
			for(int b = 0; b <= maxX; ++b)
			{
				nextGenForCell(a, b);
			}
	};

	std::vector< std::thread > threads;
	threads.reserve(std::thread::hardware_concurrency() - 1);
	while(threads.size() < threads.capacity())
		threads.emplace_back(task, maxY / (threads.capacity() - 1) * threads.size());

	for(auto& a : threads)
		a.join();
#endif
}

void CellSystem::calcNextGen()
{
	maxX = this->size() - 1;
	maxY = (*this)[0].size() - 1;

#ifndef THREADS
	for(unsigned int a = 0; a <= maxX; ++a)
	{
		for(unsigned int b = 0; b <= maxY; ++b)
		{
			nextGenForCell(a, b);
		}
	}
#else
	const auto task = [&](const int& i)
	{
		for(int a = i; a <= maxY; ++a)
			for(int b = 0; b <= maxX; ++b)
			{
				nextGenForCell(a, b);
			}
	};

	std::vector< std::thread > threads;
	threads.reserve(std::thread::hardware_concurrency() - 1 );
	while( threads.size() < threads.capacity())
		threads.emplace_back(task, maxY / (threads.capacity()) * threads.size());
	for( auto& i : threads )
		i.join();
#endif
}

void CellSystem::killAll()
{
	for(unsigned int a = 0; a <= maxX; ++a)
	{
		for(unsigned int b = 0; b <= maxY; ++b)
		{
			(*this)[a][b].alive = false;
			(*this)[a][b].next = false;
		}
	}

	generation = 0;
}

#ifdef ALG3
bool CellSystem::check(int x, int y)
{
	if( x < 0 )
		x = this->size() - ( ( -x ) % this->size() );
	else if( x >= this->size() )
		x %= this->size();
	if( y < 0 )
		y = (*this)[0].size() - ( ( -y ) % (*this)[0].size() );
	else if( y >= (*this)[0].size() )
		y %= (*this)[0].size();
	return (*this)[x][y].alive;
}
#elif defined ALG2
int CellSystem::countNeighbors(const unsigned int& x, const unsigned int& y)
{
	int total = 0;
	for (int a = -1; a <= 1; a += 1) {
		for (int b = -1; b <= 1; b += 1) {

			int i = (x + b) % (maxX + 1), j = (y + a) % (maxY + 1);

			if(i >= 0 && i <= maxX && j >= 0 && j <= maxY && !(b == 0 && a == 0) && (*this)[i][j].alive) {
				total += 1;
			}
		}
	}
	return total;
}
#endif
void CellSystem::nextGenForCell(const unsigned int& x, const unsigned int& y)
{
#ifdef ALG2
	int aliveCellNeighbs = countNeighbors(x, y);
#elif defined ALG3
	int aliveCellNeighbs = 0;

	aliveCellNeighbs += check( x - 1, y - 1 );
	aliveCellNeighbs += check( x    , y - 1 );
	aliveCellNeighbs += check( x + 1, y - 1 );
	aliveCellNeighbs += check( x - 1, y     );
	aliveCellNeighbs += check( x + 1, y     );
	aliveCellNeighbs += check( x - 1, y + 1 );
	aliveCellNeighbs += check( x    , y + 1 );
	aliveCellNeighbs += check( x + 1, y + 1 );
#elif defined ALG1
	int aliveCellNeighbs = 0;

	if(x == 0)												// left
	{
		if(y == 0)										// left + top
		{
			aliveCellNeighbs += (*this)[0][1].alive;
			aliveCellNeighbs += (*this)[1][0].alive;
			aliveCellNeighbs += (*this)[1][1].alive;

			aliveCellNeighbs += (*this)[maxX][0].alive;
			aliveCellNeighbs += (*this)[maxX][1].alive;

			aliveCellNeighbs += (*this)[maxX][maxY].alive;

			aliveCellNeighbs += (*this)[0][maxY].alive;
			aliveCellNeighbs += (*this)[1][maxY].alive;
		}
		else if(y == maxY)							// left + bottom
		{
			aliveCellNeighbs += (*this)[0][maxY - 1].alive;
			aliveCellNeighbs += (*this)[1][maxY].alive;
			aliveCellNeighbs += (*this)[1][maxY - 1].alive;

			aliveCellNeighbs += (*this)[maxX][maxY].alive;
			aliveCellNeighbs += (*this)[maxX][maxY - 1].alive;

			aliveCellNeighbs += (*this)[maxX][0].alive;

			aliveCellNeighbs += (*this)[0][0].alive;
			aliveCellNeighbs += (*this)[1][0].alive;
		}
		else										// left + mid
		{
			aliveCellNeighbs += (*this)[0][y - 1].alive;
			aliveCellNeighbs += (*this)[0][y + 1].alive;
			aliveCellNeighbs += (*this)[1][y].alive;
			aliveCellNeighbs += (*this)[1][y - 1].alive;
			aliveCellNeighbs += (*this)[1][y + 1].alive;

			aliveCellNeighbs += (*this)[maxX][y].alive;
			aliveCellNeighbs += (*this)[maxX][y - 1].alive;
			aliveCellNeighbs += (*this)[maxX][y + 1].alive;
		}


	}
	else if(x == maxX)										// right
	{
		if(y == 0)									// right + top
		{
			aliveCellNeighbs += (*this)[maxX][1].alive;
			aliveCellNeighbs += (*this)[maxX - 1][0].alive;
			aliveCellNeighbs += (*this)[maxX - 1][1].alive;

			aliveCellNeighbs += (*this)[0][0].alive;
			aliveCellNeighbs += (*this)[0][1].alive;

			aliveCellNeighbs += (*this)[maxX][maxY].alive;

			aliveCellNeighbs += (*this)[0][maxY].alive;
			aliveCellNeighbs += (*this)[1][maxY].alive;

		}
		else if(y == maxY)					// right + bottom
		{
			aliveCellNeighbs += (*this)[maxX][maxY - 1].alive;
			aliveCellNeighbs += (*this)[maxX - 1][maxY].alive;
			aliveCellNeighbs += (*this)[maxX - 1][maxY - 1].alive;

			aliveCellNeighbs += (*this)[0][maxY].alive;
			aliveCellNeighbs += (*this)[0][maxY - 1].alive;

			aliveCellNeighbs += (*this)[0][0].alive;

			aliveCellNeighbs += (*this)[maxX][0].alive;
			aliveCellNeighbs += (*this)[maxX - 1][0].alive;
		}
		else								// right + mid
		{
			aliveCellNeighbs += (*this)[maxX][y - 1].alive;
			aliveCellNeighbs += (*this)[maxX][y + 1].alive;
			aliveCellNeighbs += (*this)[maxX - 1][y].alive;
			aliveCellNeighbs += (*this)[maxX - 1][y - 1].alive;
			aliveCellNeighbs += (*this)[maxX - 1][y + 1].alive;

			aliveCellNeighbs += (*this)[0][y].alive;
			aliveCellNeighbs += (*this)[0][y - 1].alive;
			aliveCellNeighbs += (*this)[0][y + 1].alive;
		}

	}
	else if(y == 0)								// mid + top
	{
		aliveCellNeighbs += (*this)[x][1].alive;
		aliveCellNeighbs += (*this)[x - 1][0].alive;
		aliveCellNeighbs += (*this)[x - 1][1].alive;
		aliveCellNeighbs += (*this)[x + 1][0].alive;
		aliveCellNeighbs += (*this)[x + 1][1].alive;

		aliveCellNeighbs += (*this)[x][maxY].alive;
		aliveCellNeighbs += (*this)[x - 1][maxY].alive;
		aliveCellNeighbs += (*this)[x + 1][maxY].alive;
	}
	else if(y == maxY)							// mid + bottom
	{
		aliveCellNeighbs += (*this)[x][maxY - 1].alive;
		aliveCellNeighbs += (*this)[x - 1][maxY].alive;
		aliveCellNeighbs += (*this)[x - 1][maxY - 1].alive;
		aliveCellNeighbs += (*this)[x + 1][maxY].alive;
		aliveCellNeighbs += (*this)[x + 1][maxY - 1].alive;

		aliveCellNeighbs += (*this)[x][0].alive;
		aliveCellNeighbs += (*this)[x - 1][0].alive;
		aliveCellNeighbs += (*this)[x + 1][0].alive;
	}
	else										// mid + mid
	{
		aliveCellNeighbs += (*this)[x][y - 1].alive;
		aliveCellNeighbs += (*this)[x][y + 1].alive;
		aliveCellNeighbs += (*this)[x - 1][y].alive;
		aliveCellNeighbs += (*this)[x - 1][y - 1].alive;
		aliveCellNeighbs += (*this)[x - 1][y + 1].alive;
		aliveCellNeighbs += (*this)[x + 1][y].alive;
		aliveCellNeighbs += (*this)[x + 1][y - 1].alive;
		aliveCellNeighbs += (*this)[x + 1][y + 1].alive;
	}
#endif
#ifdef ALG2
	if(aliveCellNeighbs > 3)					// more than 3 = dead
	{
		(*this)[x][y].next = false;
	}
	else if(aliveCellNeighbs < 2)				// less than 2 = dead
	{
		(*this)[x][y].next = false;
	}
	else if(aliveCellNeighbs == 3)				// 3 = alive
	{
		(*this)[x][y].next = true;
	}
	else if(aliveCellNeighbs == 2 && (*this)[x][y].alive)	// if alive, 2 = alive
	{
		(*this)[x][y].next = true;
	}
	else										// if not alive, 2 = dead
	{
		(*this)[x][y].next = false;
	}
#elif defined ALG1 || defined ALG3
	(*this)[x][y].next = ( aliveCellNeighbs== 3 ) | ( ( aliveCellNeighbs== 2 ) & (*this)[x][y].alive );
#endif
}
