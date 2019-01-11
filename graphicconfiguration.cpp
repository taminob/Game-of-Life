// © Copyright (c) 2018 SqYtCO

#include "graphicconfiguration.h"
#include <fstream>
#include <filesystem>

#if __cplusplus < 201703L
namespace std
{
	namespace filesystem = experimental::filesystem;
}
#endif

GraphicConfiguration::GraphicConfiguration() : graphic_config_saved(true)
{
	// try reading in configuration; on failure set default values
	if(!read_config())
		reset_config();
}

void GraphicConfiguration::GraphicConfiguration::reset_config()
{
	language = Default_Values::LANGUAGE;
	cell_size = Default_Values::CELL_SIZE;
	grid_active = Default_Values::GRID_ACTIVE;
	generation_counter_size = Default_Values::GENERATION_COUNTER_SIZE;
	exit_warning = Default_Values::EXIT_WARNING;
	dead_color = Default_Values::DEAD_COLOR;
	alive_color = Default_Values::ALIVE_COLOR;
	reviving_color = Default_Values::REVIVING_COLOR;
	dying_color = Default_Values::DYING_COLOR;
	background_color = Default_Values::BACKGROUND_COLOR;
	fullscreen = Default_Values::FULLSCREEN;
	hide_generation_counter = Default_Values::HIDE_GENERATION_COUNTER;
	lock_after_first_generating = Default_Values::LOCK_AFTER_FIRST_GENERATING;
	left_button_alive_right_dead = Default_Values::LEFT_ALIVE_AND_RIGHT_DEAD;
	generations_per_step = Default_Values::GENERATIONS_PER_STEP;
	show_startup_dialog = Default_Values::SHOW_STARTUP_DIALOG;
	delay = Default_Values::DELAY;
}

bool GraphicConfiguration::read_config()
{
	std::ifstream in(gconfig_path + GCONFIG_FILE);

	// return on error
	if(!in)
		return false;

	// set default values if properties are missing in config_file
	reset_config();

	std::string property, value;
	// read in property name
	while(std::getline(in, property, '='))
	{
		// read in value of property
		std::getline(in, value);

		if(property == "language")
			language = static_cast<Language>(std::stoul(value));
		else if(property == "cell_size")
			cell_size = std::stoul(value);
		else if(property == "grid_active")
			grid_active = std::stoul(value);
		else if(property == "generation_counter_size")
			generation_counter_size = std::stoul(value);
		else if(property == "exit_warning")
			exit_warning = std::stoul(value);
		else if(property == "dead_color")
			dead_color = QColor(value.c_str());
		else if(property == "alive_color")
			alive_color = QColor(value.c_str());
		else if(property == "reviving_color")
			reviving_color = QColor(value.c_str());
		else if(property == "dying_color")
			dying_color = QColor(value.c_str());
		else if(property == "background_color")
			background_color = QColor(value.c_str());
		else if(property == "fullscreen")
			fullscreen = std::stoul(value);
		else if(property == "hide_generation_counter")
			hide_generation_counter = std::stoul(value);
		else if(property == "lock_after_first_generating")
			lock_after_first_generating = std::stoul(value);
		else if(property == "left_button_alive_right_dead")
			left_button_alive_right_dead = std::stoul(value);
		else if(property == "generations_per_step")
			generations_per_step = std::stoul(value);
		else if(property == "show_startup_dialog")
			show_startup_dialog = std::stoul(value);
		else if(property == "delay")
			delay = std::stoul(value);
	}

	// return on error
	if(!in)
		return false;

	graphic_config_saved = true;

	// return on success
	return true;
}

bool GraphicConfiguration::write_config()
{
	try
	{
		// check if path exists and create it if not
		if(!std::filesystem::exists(gconfig_path))
			std::filesystem::create_directories(gconfig_path);
	}
	catch(...)	// if write permission is not granted
	{
		return false;
	}

	std::ofstream out(gconfig_path + GCONFIG_FILE);

	// return on error
	if(!out)
		return false;

	out << "language=" << static_cast<int>(language) << '\n'
	<< "cell_size=" << cell_size << '\n'
	<< "grid_active=" << grid_active << '\n'
	<< "generation_counter_size=" << generation_counter_size << '\n'
	<< "exit_warning=" << exit_warning << '\n'
	<< "dead_color=" << dead_color.name().toStdString() << '\n'
	<< "alive_color=" << alive_color.name().toStdString() << '\n'
	<< "reviving_color=" << reviving_color.name().toStdString() << '\n'
	<< "dying_color=" << dying_color.name().toStdString() << '\n'
	<< "background_color=" << background_color.name().toStdString() << '\n'
	<< "fullscreen=" << fullscreen << '\n'
	<< "hide_generation_counter=" << hide_generation_counter << '\n'
	<< "lock_after_first_generating=" << lock_after_first_generating << '\n'
	<< "left_button_alive_right_dead=" << left_button_alive_right_dead << '\n'
	<< "generations_per_step=" << generations_per_step << '\n'
	<< "show_startup_dialog=" << show_startup_dialog << '\n'
	<< "delay=" << delay;

	// return on error
	if(!out)
		return false;

	graphic_config_saved = true;

	return true;
}
