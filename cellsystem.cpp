// © Copyright (c) 2018 SqYtCO

#include "cellsystem.h"
#include <random>

Cell_System::Cell_System(const std::size_t& columns, const std::size_t& rows, const Border_Behavior& border_behavior,
			const std::size_t& live_rules, const std::size_t& reborn_rules, const std::size_t& threads) : Base_System(columns, rows), current_system(0),
																											border_behavior(border_behavior),
																											live_rules(live_rules), reborn_rules(reborn_rules)
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
		system[current_system][columns * rows].state = static_cast<Cell_State>(border_behavior == Border_Behavior::Surrounded_By_Living_Cells);
		system[current_system ^ 0x01][columns * rows].state = static_cast<Cell_State>(border_behavior == Border_Behavior::Surrounded_By_Living_Cells);
	}

	// set all cells to dead
	set_all(Dead);
}

void Cell_System::set_num_of_threads(const std::size_t& threads)
{
	// only set threads if they can increase performance
	if(num_of_threads < size_y / 2)
		// at least 1 thread
		num_of_threads = (threads != 0) ? threads : 1;
	else
		num_of_threads = 1;
}

void Cell_System::calc_cell_opposite_behavior(const std::size_t& x, const std::size_t& y)
{
	std::size_t alive_neighbors = 0;

	// state of given cell
	Cell_State state = system[current_system][y * size_x + x].state;

	for(int a = -1; a < 2; ++a)
		for(int b = -1; b < 2; ++b)
			// sum alive neighbors and call algorithm for calculating neighbors of border-cells
			alive_neighbors += system[current_system][(get_opposite_on_border_y(static_cast<long>(y) + a) * size_x) + get_opposite_on_border_x(static_cast<long>(x) + b)].state;

	alive_neighbors -= state;

	// update state in next system; fastest way to calculate next state with standard rule set
//	system[current_system ^ 0x01][y * size_x + x].state = static_cast<Cell_State>((alive_neighbors == 3 ) | ((alive_neighbors == 2) & state));

	// if cell is alive, try live_rules
	if(state)
	{
		if(live_rules & (1 << alive_neighbors))
			system[current_system ^ 0x01][y * size_x + x].state = Alive;
		else
			system[current_system ^ 0x01][y * size_x + x].state = Dead;
	}
	// if cell is dead, try reborn_rules
	else if(reborn_rules & (1 << alive_neighbors))
		system[current_system ^ 0x01][y * size_x + x].state = Alive;
	else
		system[current_system ^ 0x01][y * size_x + x].state = Dead;
}

void Cell_System::calc_cell_special_border_behavior(const std::size_t& x, const std::size_t& y)
{
	std::size_t alive_neighbors = 0;

	// state of given cell
	Cell_State state = system[current_system][y * size_x + x].state;

	for(int a = -1; a < 2; ++a)
		for(int b = -1; b < 2; ++b)
			// sum alive neighbors and call algorithm for calculating neighbors of border-cells
			alive_neighbors += system[current_system][get_special_border(x + b, y + a)].state;

	alive_neighbors -= state;

	// update state in next system; fastest way to calculate next state with standard rule set
//	system[current_system ^ 0x01][y * size_x + x].state = static_cast<Cell_State>((alive_neighbors == 3 ) | ((alive_neighbors == 2) & state));

	// if cell is alive, try live_rules
	if(state)
	{
		if(live_rules & (1 << alive_neighbors))
			system[current_system ^ 0x01][y * size_x + x].state = Alive;
		else
			system[current_system ^ 0x01][y * size_x + x].state = Dead;
	}
	// if cell is dead, try reborn_rules
	else if(reborn_rules & (1 << alive_neighbors))
		system[current_system ^ 0x01][y * size_x + x].state = Alive;
	else
		system[current_system ^ 0x01][y * size_x + x].state = Dead;
}

void Cell_System::calc_part(const std::size_t& max_x, const std::size_t& min_x, const std::size_t& max_y, const std::size_t& min_y)
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
	threads.reserve(num_of_threads);
	// calc rows per thread
	std::size_t y_per_job = size_y / num_of_threads;
	// calc does not work out evenly
	std::size_t odd_ys = size_y % num_of_threads;

	if(num_of_threads == 1)
		odd_ys = size_y;
	else
	{
		// start set amount of threads with given y_per_job
		for(std::size_t i = 0; i < num_of_threads; ++i)
			threads.emplace_back(std::thread(&Cell_System::calc_part, this, size_x, 0, y_per_job * (i + 1) + odd_ys, y_per_job * i + odd_ys));
	}

	calc_part(size_x, 0, odd_ys, 0);

	for(std::size_t i = 0; i < threads.size(); ++i)
		threads[i].join();
	threads.clear();
}

void Cell_System::next_generation()
{
	// update state to next_state: change system to next generation (precalculated)
	current_system ^= 0x01;

	// calculate new next_state
	calc_next_generation();
}

void Cell_System::random_cells(const std::size_t& alive, const std::size_t& dead)
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

void Cell_System::set_cell(const std::size_t& x, const std::size_t& y, const Cell_State& state)
{
	system[current_system][y * size_x + x].state = state;
	system[current_system ^ 0x01][y * size_x + x].state = state;
}

void Cell_System::set_all(const Cell_State& state)
{
	for(std::size_t row = 0; row < size_y; ++row)
		for(std::size_t column = 0; column < size_x; ++column)
			system[current_system][row * size_x + column].state = state;

	// calc next state for all cells
	calc_next_generation();
}
