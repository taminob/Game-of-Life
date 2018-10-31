// © Copyright (c) 2018 SqYtCO

#include "configuration.h"
#include <string>
#include <fstream>
#include <experimental/filesystem>

Configuration::Configuration() : config_saved(true)
{
	// try reading in configuration; on failure set default values
	if(!read_config())
		reset_config();
}

void Configuration::reset_config()
{
	// set all default values
	size_x = Default_Values::SIZE_X;
	size_y = Default_Values::SIZE_Y;
	num_of_threads = Default_Values::NUM_OF_THREADS;
	relation_dead = Default_Values::RELATION_DEAD;
	relation_alive = Default_Values::RELATION_ALIVE;
	border_behavior = Default_Values::BORDER_BEHAVIOR;
	save_path = Default_Values::SAVE_PATH;
	start_random = Default_Values::START_RANDOM;
	survival_rules = Default_Values::SURVIVAL_RULES;
	rebirth_rules = Default_Values::REBORN_RULES;
}

bool Configuration::read_config()
{
	std::ifstream in(config_path + CONFIG_FILE);

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

		if(property == "size_x")
			size_x = std::stoul(value);
		else if(property == "size_y")
			size_y = std::stoul(value);
		else if(property == "num_of_threads")
			num_of_threads = std::stoul(value);
		else if(property == "relation_dead")
			relation_dead = std::stoul(value);
		else if(property == "relation_alive")
			relation_alive = std::stoul(value);
		else if(property == "border_behavior")
			border_behavior = static_cast<Border_Behavior>(std::stoul(value));
		else if(property == "save_path")
			save_path = value;
		else if(property == "start_random")
			start_random = std::stoul(value);
		else if(property == "survival_rules")
			survival_rules = std::stoul(value);
		else if(property == "rebirth_rules")
			rebirth_rules = std::stoul(value);
	}

	config_saved = true;

	// return on success
	return true;
}

bool Configuration::write_config()
{
	try
	{
		// check if path exists and create it if not
		if(!std::experimental::filesystem::exists(config_path))
			std::experimental::filesystem::create_directories(config_path);
	}
	catch(...)	// if write permission is not granted
	{
		return false;
	}

	std::ofstream out(config_path + CONFIG_FILE);

	// return on error
	if(!out)
		return false;

	out << "size_x=" << size_x << '\n'
		<< "size_y=" << size_y << '\n'
		<< "num_of_threads=" << num_of_threads << '\n'
		<< "relation_dead=" << relation_dead << '\n'
		<< "relation_alive=" << relation_alive << '\n'
		<< "border_behavior=" << static_cast<int>(border_behavior) << '\n'
		<< "save_path=" << save_path << '\n'
		<< "start_random=" << start_random << '\n'
		<< "survival_rules=" << survival_rules << '\n'
		<< "rebirth_rules=" << rebirth_rules;

	// return on error
	if(!out)
		return false;

	config_saved = true;

	return true;
}
