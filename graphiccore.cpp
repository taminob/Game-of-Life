// © Copyright (c) 2018 SqYtCO

#include "graphiccore.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#ifdef ENABLE_CALC_TIME_MEASUREMENT
#include <QDebug>
#include <chrono>
#endif

GraphicCore* GraphicCore::get_instance()
{
	static GraphicCore gcore;

	return &gcore;
}

GraphicCore::GraphicCore()
{
	// get correct config path
	QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	if(config_path[config_path.size() - 1] != QDir::separator())
		config_path.append(QDir::separator());

	// set config paths
	Core::get_instance()->get_config()->set_config_path(config_path.toStdString());
	gconfig.set_config_path(config_path.toStdString());

	// create new system with correct configuration
	Core::get_instance()->new_system();

	opengl = new OpenGLWidget(&parent);
	gen_counter = new QLabel(&parent);
	gen_counter->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	gen_counter->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	gen_counter->setWindowFlags(Qt::SubWindow);
	gen_counter->setForegroundRole(QPalette::BrightText);
	update_generation_counter();
}

void GraphicCore::next_generation()
{
#ifdef ENABLE_CALC_TIME_MEASUREMENT
	auto begin = std::chrono::high_resolution_clock::now();
#endif
	gen_counter->setNum(static_cast<int>(Core::get_instance()->next_generation()));
#ifdef ENABLE_CALC_TIME_MEASUREMENT
	auto end = std::chrono::high_resolution_clock::now();
	qDebug() << "calculating: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs";
	update_opengl();
#endif
}

void GraphicCore::reset_cells(const Cell_State& state)
{
	Core::get_instance()->reset_cells(state);
	update_generation_counter();
	update_opengl();
}

void GraphicCore::new_system()
{
	opengl->stop_step();
	opengl->stop_generating();
	// generate new system and fill with random cells if it is set
	Core::get_instance()->new_system();
	update_generation_counter();
	update_opengl();
}

void GraphicCore::reset_movement()
{
	opengl->reset_movement();
}

bool GraphicCore::get_generating_running()
{
	return opengl->get_generating_running();
}

void GraphicCore::start_generating()
{
	opengl->start_generating();
}

void GraphicCore::stop_generating()
{
	opengl->stop_generating();
}

void GraphicCore::update_generation_counter()
{
	gen_counter->setVisible(!gconfig.get_hide_generation_counter());
	gen_counter->setFont(QFont("", get_config()->get_generation_counter_size()));
	gen_counter->setNum(static_cast<int>(Core::get_instance()->get_generation()));
}

void GraphicCore::read_save()
{
	QString selected_filter("Game Of Life(*.gol)");
	Core::get_instance()->load(QFileDialog::getOpenFileName(nullptr, QFileDialog::tr("Select a file to open..."), Core::get_instance()->get_config()->get_save_path().c_str(), QFileDialog::tr("All Files(*);;Game Of Life(*.gol)"), &selected_filter).toStdString());
	update_opengl();
	update_generation_counter();
}

void GraphicCore::write_save_as()
{
	// ask for file name
	QString selected_filter("Game Of Life(*.gol)");
	QString file_name = QFileDialog::getSaveFileName(nullptr, QFileDialog::tr("Choose a file name to save..."), Core::get_instance()->get_config()->get_save_path().c_str(), QFileDialog::tr("All Files(*);;Game Of Life(*.gol)"), &selected_filter);

	// return if no file name was entered
	if(file_name.isEmpty())
		return;

	// write save; warning if writing fails
	if(!Core::get_instance()->save(file_name.toStdString()))
		QMessageBox::warning(nullptr, QMessageBox::tr("Write Error"), QMessageBox::tr("Writing Save Failed!\nPlease Check Your Permissions."));
}

void GraphicCore::write_save()
{
	// write save; warning if writing fails
	if(!Core::get_instance()->save())
		QMessageBox::warning(nullptr, QMessageBox::tr("Write Error"), QMessageBox::tr("Writing Save Failed!\nPlease Check Your Permissions."));
}
