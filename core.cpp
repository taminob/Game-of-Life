// © Copyright (c) 2018 SqYtCO

#include "core.h"
#include "cellsystem.h"
#include "hashlifesystem.h"
#include <fstream>

#if __cplusplus < 201703L
#include <experimental/filesystem>
namespace std
{
	namespace filesystem = experimental::filesystem;
}
#else
#include <filesystem>
#endif


Configuration Core::config;
std::unique_ptr<Base_System> Core::system_;
std::size_t Core::generation;

std::size_t Core::next_generation(std::size_t generations)
{
	HashLife_System* temp = dynamic_cast<HashLife_System*>(system_.get());
	if(temp)
	{
		generation += temp->next_generation(generations);
		return generations;
	}

	system_->next_generation();
	++generation;
	return 1;
}

void Core::calc_next_generation(std::size_t generations)
{
	HashLife_System* temp = dynamic_cast<HashLife_System*>(system_.get());
	if(temp)
		temp->calc_next_generation(generations);
	else
		system_->calc_next_generation();
}

bool Core::save(const std::string& file)
{
	std::ofstream out(file);
	// if creating fails
	if(!out)
		return false;

	out << generation << '\n';

	for(std::size_t row = 0; row < config.get_size_y(); ++row)
	{
		for(std::size_t column = 0; column < config.get_size_x(); ++column)
		{
			out << static_cast<int>(system_->get_cell_state(column, row)) << ' ';
		}

		out << '\n';
	}

	// if writing fails
	if(!out)
		return false;

	return true;
}

bool Core::load(const std::string& file)
{
	// return on empty file
	if(file.empty())
		return false;

	// check if ending is equal to ".gol"; if not, return false
	if(file.substr(file.size() - 4, 4) != ".gol")
		return false;

	std::ifstream in(file);
	// if reading fails, return false
	if(!in)
		return false;

	// skip saved generation num
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// count size; size_x is determined by the longest row
	std::string temp_s;
	char temp_c;
	std::size_t size_x_counter = 0, size_x_max = 0, size_y = 0;
	while(in)
	{
		// read in line
		std::getline(in, temp_s);

		// count columns
		for(std::size_t i = 0; i < temp_s.size(); ++i)
		{
			temp_c = temp_s[i];

			// count only valid cell states
			if(temp_c == '1' || temp_c == '0')
				++size_x_counter;
		}

		// count (filles) rows
		if(!temp_s.empty())
			++size_y;

		// longest row (most columns) determinates size_x
		if(size_x_counter > size_x_max)
			size_x_max = size_x_counter;

		// reset column counter
		size_x_counter = 0;
	}

	// reopen file to reset read position and EOF-state
	in.close();
	in.open(file);

	// create new Cell_System
	system_.reset(new Cell_System(size_x_max, size_y, config.get_border_behavior(), config.get_survival_rules(), config.get_rebirth_rules(), config.get_num_of_threads()));

	// read in saved generation
	std::size_t saved_generation;
	in >> saved_generation;

	// read in cell states
	std::size_t column = 0, row = 0;
	while(in)
	{
		// read in line
		std::getline(in, temp_s);

		// read states
		for(std::size_t i = 0; i < temp_s.size(); ++i)
		{
			temp_c = temp_s[i];

			// set cell to read state
			if(temp_c == '1' || temp_c == '0')
				system_->set_cell(column++, row, static_cast<Cell_State>(temp_c - '0'));
		}

		// count up rows
		if(!temp_s.empty())
			++row;

		column = 0;
	}

	// set current generation to read generation
	generation = saved_generation;
	// calc next generation
	system_->calc_next_generation();

	// return true on success
	return true;
}

void Core::new_system()
{
	if(config.get_border_behavior() == Border_Behavior::Borderless)
		system_.reset(new HashLife_System(config.get_survival_rules(), config.get_rebirth_rules()));
	else
		system_.reset(new Cell_System(config.get_size_x(), config.get_size_y(), config.get_border_behavior(),
								config.get_survival_rules(), config.get_rebirth_rules(), config.get_num_of_threads()));

	if(config.get_start_random())
		system_->random_cells(config.get_relation_alive(), config.get_relation_dead());
	generation = 0;
}

std::size_t Core::get_num_of_alive_cells()
{
	auto temp = dynamic_cast<HashLife_System*>(system_.get());
	if(temp)
		return temp->get_num_of_alive_cells();
	else
		return 0;
}

bool Core::expand()
{
	auto temp = dynamic_cast<HashLife_System*>(system_.get());
	if(temp)
		temp->resize(temp->get_size_x() * 2);
	else
		return false;
	return true;
}
