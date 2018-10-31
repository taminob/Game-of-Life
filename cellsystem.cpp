// © Copyright (c) 2018 SqYtCO

#include "cellsystem.h"
#include <random>

Cell_System::Cell_System(std::size_t columns, std::size_t rows, const Border_Behavior& border_behavior,
			std::size_t survival_rules, std::size_t rebirth_rules, std::size_t threads) : Base_System(columns, rows), current_system(0),
																											survival_rules(survival_rules), rebirth_rules(rebirth_rules),
																											border_behavior(border_behavior)
{
	// check given num and set num_of_threads
	set_num_of_threads(threads);

	if(border_behavior == Border_Behavior::Continue_On_Opposite_Side)	// "opposite"-behavior
	{
		// create cells for current state
		system[current_system].resize(columns * rows);
		// create cells for next state
		system[current_system ^ 0x01].resize(columns * rows);
	}
	else	// "special border"-behavior
	{
		// create cells for current state; last cell reserved for border behavior "Surrounded_By_Dead_Cells/Surrounded_By_Living_Cells"
		system[current_system].resize(columns * rows + 1);
		// create cells for next state; last cell reserved for border behavior "Surrounded_By_Dead_Cells/Surrounded_By_Living_Cells"
		system[current_system ^ 0x01].resize(columns * rows + 1);

		// set last cell for special border behavior to alive or dead
		system[current_system][columns * rows] = static_cast<Cell_State>(border_behavior == Border_Behavior::Surrounded_By_Living_Cells);
		system[current_system ^ 0x01][columns * rows] = static_cast<Cell_State>(border_behavior == Border_Behavior::Surrounded_By_Living_Cells);
	}

	// set all cells to dead
	set_all(Dead);
}

void Cell_System::set_num_of_threads(std::size_t threads)
{
	// only set threads if they can increase performance
	if(num_of_threads < size_y / 4)
		// at least 1 thread
		num_of_threads = (threads != 0) ? threads : 1;
	else
		num_of_threads = 1;

	this->threads.resize(num_of_threads - 1);
}

void Cell_System::calc_cell_opposite_behavior(std::size_t x, std::size_t y)
{
	std::size_t alive_neighbors = 0;

	// state of given cell
	Cell_State state = system[current_system][y * size_x + x];

	for(int a = -1; a < 2; ++a)
		for(int b = -1; b < 2; ++b)
			// sum alive neighbors and call algorithm for calculating neighbors of border-cells
			alive_neighbors += system[current_system][(get_opposite_on_border_y(static_cast<long>(y) + a) * size_x) + get_opposite_on_border_x(static_cast<long>(x) + b)];

	alive_neighbors -= state;

	// update state in next system; fastest way to calculate next state with standard rule set
//	system[current_system ^ 0x01][y * size_x + x] = static_cast<Cell_State>((alive_neighbors == 3 ) | ((alive_neighbors == 2) & state));

	// if cell is alive, try survival_rules
	if(state)
	{
		if(survival_rules & (1 << alive_neighbors))
			system[current_system ^ 0x01][y * size_x + x] = Alive;
		else
			system[current_system ^ 0x01][y * size_x + x] = Dead;
	}
	// if cell is dead, try rebirth_rules
	else if(rebirth_rules & (1 << alive_neighbors))
		system[current_system ^ 0x01][y * size_x + x] = Alive;
	else
		system[current_system ^ 0x01][y * size_x + x] = Dead;
}

void Cell_System::calc_cell_special_border_behavior(std::size_t x, std::size_t y)
{
	std::size_t alive_neighbors = 0;

	// state of given cell
	Cell_State state = system[current_system][y * size_x + x];

	for(int a = -1; a < 2; ++a)
		for(int b = -1; b < 2; ++b)
			// sum alive neighbors and call algorithm for calculating neighbors of border-cells
			alive_neighbors += system[current_system][get_special_border(x + b, y + a)];

	alive_neighbors -= state;

	// update state in next system; fastest way to calculate next state with standard rule set
//	system[current_system ^ 0x01][y * size_x + x] = static_cast<Cell_State>((alive_neighbors == 3 ) | ((alive_neighbors == 2) & state));

	// if cell is alive, try survival_rules
	if(state)
	{
		if(survival_rules & (1 << alive_neighbors))
			system[current_system ^ 0x01][y * size_x + x] = Alive;
		else
			system[current_system ^ 0x01][y * size_x + x] = Dead;
	}
	// if cell is dead, try rebirth_rules
	else if(rebirth_rules & (1 << alive_neighbors))
		system[current_system ^ 0x01][y * size_x + x] = Alive;
	else
		system[current_system ^ 0x01][y * size_x + x] = Dead;
}

void Cell_System::calc_part(std::size_t max_x, std::size_t min_x, std::size_t max_y, std::size_t min_y)
{
	std::size_t x = max_x, y = max_y;

	// if statement outside of loop to improve performance
	if(border_behavior == Continue_On_Opposite_Side)
	{
		while(y-- > min_y)
		{
			while(x-- > min_x)
				calc_cell_opposite_behavior(x, y);

			// reset x
			x = max_x;
		}
	}
	else
	{
		while(y-- > min_y)
		{
			while(x-- > min_x)
				calc_cell_special_border_behavior(x, y);

			// reset x
			x = max_x;
		}
	}
}

void Cell_System::calc_next_generation()
{
	// calc rows per thread
	std::size_t y_per_job = size_y / num_of_threads;
	// calc does not work out evenly
	std::size_t odd_ys = size_y % num_of_threads;

	if(num_of_threads == 1)
		odd_ys = size_y;
	else
	{
		// start set amount of threads with given y_per_job
		for(std::size_t i = 0; i < threads.size(); ++i)
			threads[i] = std::thread(&Cell_System::calc_part, this, size_x, 0, y_per_job * (i + 1) + odd_ys, y_per_job * i + odd_ys);
	}

	calc_part(size_x, 0, odd_ys, 0);

	for(std::size_t i = 0; i < threads.size(); ++i)
		threads[i].join();
}

void Cell_System::next_generation()
{
	// update state to next_state: change system to next generation (precalculated)
	current_system ^= 0x01;

	// calculate new next_state
	calc_next_generation();
}

void Cell_System::random_cells(std::size_t alive, std::size_t dead)
{
	// initialize random generation; generate values between 0 and ("alive" + "dead") for right ratio
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<std::size_t> dist(0, alive + dead - 1);

	for(std::size_t row = 0; row < size_y; ++row)
		for(std::size_t column = 0; column < size_x; ++column)
			// if generated random value is lower than "alive", the cell lifes (ratio: "alive"/"dead")
			set_cell(column, row, static_cast<Cell_State>(dist(mt) < alive));

	calc_next_generation();
}

void Cell_System::set_cell(std::size_t x, std::size_t y, Cell_State state)
{
	system[current_system][y * size_x + x] = state;
	system[current_system ^ 0x01][y * size_x + x] = state;
}

void Cell_System::set_all(Cell_State state)
{
	for(std::size_t row = 0; row < size_y; ++row)
		for(std::size_t column = 0; column < size_x; ++column)
			system[current_system][row * size_x + column] = state;

	// calc next state for all cells
	calc_next_generation();
}
