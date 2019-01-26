// © Copyright (c) 2018 SqYtCO

#include "graphiccore.h"
#include "core.h"
#include "openglwidget.h"
#include "hashlifesystem.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLabel>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>					// std::put_time
#ifdef ENABLE_CALC_TIME_MEASUREMENT
#include <QDebug>
#endif

GraphicConfiguration GraphicCore::gconfig;
OpenGLWidget* GraphicCore::opengl;
QLabel* GraphicCore::gen_counter;
std::unique_ptr<std::thread> GraphicCore::stepping_thread;
std::atomic_bool GraphicCore::stepping_stop;
bool GraphicCore::stepping_block;
std::unique_ptr<std::thread> GraphicCore::generating_thread;
std::atomic_bool GraphicCore::generating_stop;
std::unique_ptr<std::thread> GraphicCore::calc_thread;
std::atomic_bool GraphicCore::calc_stop;
std::mutex GraphicCore::system_mutex;

void GraphicCore::init()
{
	// get correct config path
	QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	if(config_path[config_path.size() - 1] != '/')
		config_path.append('/');

	// set config paths
	Core::get_config()->set_config_path(config_path.toStdString());
	gconfig.set_config_path(config_path.toStdString());

	// create new system with correct configuration
	Core::new_system();

	stepping_stop = true;
	stepping_block = false;

	generating_stop = true;
}

void GraphicCore::init_gui(OpenGLWidget* opengl, QLabel* gen_counter)
{
	GraphicCore::opengl = opengl;

	GraphicCore::gen_counter = gen_counter;
	gen_counter->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	gen_counter->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	gen_counter->setWindowFlags(Qt::SubWindow);
	gen_counter->setForegroundRole(QPalette::BrightText);
	update_generation_counter();
}

void GraphicCore::reset_cells(Cell_State state)
{
	Core::reset_cells(state);
	update_generation_counter();
	update_opengl();
}

void GraphicCore::new_system()
{
	stop_step();
	stop_generating();
	wait_for_calculation();

	// generate new system and fill with random cells if it is set
	Core::new_system();

	update_generation_counter();
	update_opengl();
}

void GraphicCore::reset_movement()
{
	opengl->reset_movement();
}

void GraphicCore::update_generation_counter()
{
	gen_counter->setVisible(!gconfig.get_hide_generation_counter());
	gen_counter->setFont(QFont("", static_cast<int>(get_config()->get_generation_counter_size())));
	gen_counter->setText(QString::number(Core::get_generation()));
}

void GraphicCore::update_opengl()
{
	opengl->update();
}

void GraphicCore::read_save()
{
	QString selected_filter("Game of Life(*.gol)");
	Core::load(QFileDialog::getOpenFileName(nullptr, QFileDialog::tr("Select a file to open..."), GraphicCore::get_config()->get_save_path().c_str(), QFileDialog::tr("All Files(*);;Game of Life(*.gol)"), &selected_filter).toStdString());
	update_opengl();
	update_generation_counter();
}

void GraphicCore::write_save_as()
{
	// ask for file name
	QString selected_filter("Game of Life(*.gol)");
	QString file_name = QFileDialog::getSaveFileName(nullptr, QFileDialog::tr("Choose a file name to save..."), GraphicCore::get_config()->get_save_path().c_str(), QFileDialog::tr("All Files(*);;Game of Life(*.gol)"), &selected_filter);

	// return if no file name was entered
	if(file_name.isEmpty())
		return;

	// write save; warning if writing fails
	if(!Core::save(file_name.toStdString()))
		QMessageBox::warning(nullptr, QMessageBox::tr("Write Error"), QMessageBox::tr("Writing Save Failed!\nPlease Check Your Permissions."));
}

void GraphicCore::write_save()
{
	try
	{
		// check if path exists and create it if not
		if(!std::filesystem::exists(gconfig.get_save_path()) && !gconfig.get_save_path().empty())
		{
			std::string save_dir = gconfig.get_save_path();
			if(save_dir.back() == std::filesystem::path::preferred_separator)
				save_dir.pop_back();
			if(!std::filesystem::create_directories(save_dir))
				throw std::runtime_error("");
		}

		std::string file;
		// create string with current date
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string date;
		std::ostringstream oss;
		oss << std::put_time(std::localtime(&time), "%F_%H-%M");	// %y-%m-%d_%H-%M = YY-MM-DD_HH-min
		date = oss.str();

		// attached num if file exists already
		std::size_t save_num = 0;
		while(true)
		{
			// first try without num
			if(save_num == 0)
				file = gconfig.get_save_path() + date + ".gol";
			else
				file = gconfig.get_save_path() + date + "_" + std::to_string(save_num) + ".gol";

			// check if file exists already
			std::ifstream in(file);
			// if file does not exist, break loop
			if(!in)
				break;
			else
				++save_num;
		}

		// write save; warning if writing fails
		if(!Core::save(file))
			throw std::runtime_error("");
	}
	catch(...)	// if write permission is not granted
	{
		QMessageBox::warning(nullptr, QMessageBox::tr("Write Error"), QMessageBox::tr("Writing Save Failed!\nPlease Check Your Permissions."));
	}
}

void GraphicCore::next_generations(std::size_t generations)
{
	while(generations)
	{
		if(stepping_stop)
			break;
		else
		{
			{
				std::lock_guard<decltype(system_mutex)> lock(system_mutex);
				generations -= Core::next_generation(generations);
			}
		}
	}

	stepping_stop = true;

	// send signals to main thread (update GUI)
	emit opengl->cell_changed();
	emit opengl->start_update();
}

void GraphicCore::step()
{
	wait_for_calculation();
	// stop last step
	stop_step();

	// if thread is not blocked, start new step
	if(!stepping_block)
	{
		stepping_stop = false;
		stepping_thread.reset(new std::thread(next_generations, gconfig.get_generations_per_step()));
	}
}

void GraphicCore::stop_step()
{
	stepping_stop = true;
	if(stepping_thread)
	{
		stepping_thread->join();
		stepping_thread.reset(nullptr);
	}
}

void GraphicCore::start_generating()
{
	if(generating_running())
		return;

	wait_for_calculation();
	stop_step();
	stepping_block = true;

	if(generating_thread)
		generating_thread->join();

	generating_stop = false;
	generating_thread.reset(new std::thread([&]()
	{
		while(!generating_stop)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(gconfig.get_delay()));

#ifdef ENABLE_CALC_TIME_MEASUREMENT
			auto begin = std::chrono::high_resolution_clock::now();
#endif
			{
				std::lock_guard<decltype(system_mutex)> lock(system_mutex);
				Core::next_generation();
			}
#ifdef ENABLE_CALC_TIME_MEASUREMENT
			auto end = std::chrono::high_resolution_clock::now();
			qDebug() << "calculating: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs";
#endif

			emit opengl->cell_changed();
			emit opengl->start_update();
		}
	}));

	emit opengl->generating_start_stop();
}

void GraphicCore::stop_generating()
{
	if(generating_stop)
		return;

	generating_stop = true;

	if(generating_thread)
		generating_thread->join();
	generating_thread.reset(nullptr);
	stepping_block = false;

	emit opengl->generating_start_stop();
}

void GraphicCore::calc_next_generation()
{
	if(generating_running())
		return;

	wait_for_calculation();

	calc_thread.reset(new std::thread([]()
	{
		{
			std::lock_guard<decltype(system_mutex)> lock(system_mutex);
			Core::calc_next_generation(gconfig.get_generations_per_step());
		}
		emit opengl->start_update();
	}));
}

void GraphicCore::wait_for_calculation()
{
	if(calc_thread)
	{
		calc_thread->join();
		calc_thread.reset(nullptr);
	}
}
