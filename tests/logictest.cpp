#include "../cell.h"
#include "../cellsystem.h"
#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <cstdlib>
#include <utility>

struct Datas
{
	int x = 10;
	int y = 10;
	int gens = 5000;
};

Datas analyzeArgs(int argc, char** argv);

int main(int argc, char* argv[])
{
	Datas data;
	if(argc > 1)
	{
		data = analyzeArgs(argc, argv);
	}

	CellSystem system;
	std::vector<Cell> row;
	std::mt19937 mt(std::random_device{ }());
	std::uniform_int_distribution<int> dist(0, 2);

	for(int a = 0; a < data.y; ++a)
	{
		for(int b = 0; b < data.x; ++b)
		{
			if(dist(mt))
				row.push_back(Cell(false));
			else
				row.push_back(Cell(true));
		}
		system.push_back(std::move(row));
		row.clear();
	}

	system.calcNextGen();

	for(int i = 0; i < data.gens; ++i)
	{
		std::cout << "Generation " << system.getGeneration() << ":\n";
		for(int a = 0; a < system.size(); ++a)
		{
			for(int b = 0; b < system.at(0).size(); ++b)
			{
				std::cout << system.at(a).at(b).alive << ' ';
			}
			std::cout << '\n';
		}

		char c = std::cin.get();
		if(c == '\n')
			system.nextGen();
		else
			break;
	}

	return 0;
}

Datas analyzeArgs(int argc, char** argv)
{
	Datas temp;
	for(int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);
		if(arg == "-x" && i < argc)
		{
			std::string x_arg(argv[++i]);
			temp.x = 0;
			for(int a = 0; a < x_arg.length(); ++a)
			{
				if(std::isdigit(x_arg[a]))
				{
					temp.x = (x_arg[a] - '0') + 10 * temp.x;
				}
				else
				{
					std::cout << "Bad -x argument!\n\n";
					exit(-1);
				}
			}
		}
		else if(arg == "-y" && i < argc)
		{
			std::string y_arg(argv[++i]);
			temp.y = 0;
			for(int a = 0; a < y_arg.length(); ++a)
			{
				if(std::isdigit(y_arg[a]))
				{
					temp.y = (y_arg[a] - '0') + 10 * temp.y;
				}
				else
				{
					std::cout << "Bad -y argument!\n\n";
					exit(-1);
				}
			}
		}
		else if(arg == "-g" && i < argc)
		{
			std::string g_arg(argv[++i]);
			temp.gens = 0;
			for(int a = 0; a < g_arg.length(); ++a)
			{
				if(std::isdigit(g_arg[a]))
				{
					temp.gens = (g_arg[a] - '0') + 10 * temp.gens;
				}
				else
				{
					std::cout << "Bad -y argument!\n\n";
					exit(-1);
				}
			}
		}
		else
		{
			std::cout << argv[i] << ": Invalid argument!\n\n";
			exit(-1);
		}
	}

	return temp;
}
