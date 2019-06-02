#include "game_time.h"
#include <thread>

GameTime::GameTime(bool fixedTimestep, uint32_t targetFps) {
	_is_fixed = fixedTimestep;
	setTargetFps(targetFps);
	_fps = 0;
	_total_time = 0;
	_frame_id = 0;
	_frame_time = 0;
	_delta = 0;
	_accumulated = 0;
	_prev_time = std::chrono::high_resolution_clock::now();
}

GameTime::~GameTime() {

}

bool GameTime::getPaused() {
	return _paused;
}

void GameTime::setPaused(bool paused) {
	_paused = paused;
}

uint32_t GameTime::getTargetFps() {
	return _target_fps;
}

void GameTime::setTargetFps(uint32_t fps) {
	_target_fps = fps;
	_target_time = 1000.0 / _target_fps;
}

bool GameTime::getFixedTimestep() {
	return _is_fixed;
}

void GameTime::setFixedTimestep(bool fixed) {
	_is_fixed = fixed;
}

uint32_t GameTime::getFps() {
	return _fps;
}

double GameTime::getFrameTime() {
	return _frame_time;
}

double GameTime::getTargetFrameTime() {
	return _target_time;
}

double GameTime::getTotalTime() {
	return _total_time;
}

uint64_t GameTime::getFrameId() {
	return _frame_id;
}

double GameTime::getDelta() {
	return _delta;
}

uint32_t GameTime::tick() {
	// TODO: Implement custom sleep() func to pad times and "sleep" most of it away then spinway for the padded amount.
	//		 This is to help reduce CPU usage from ticking endlessly during fixed-timestep.
	// http://www.cplusplus.com/forum/windows/33865/
	auto time = std::chrono::high_resolution_clock::now();
	auto time_elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time - _prev_time);
	double elapsed = time_elapsed.count();

	uint32_t required_frames = 0;

	if (_is_fixed) { // Fixed timestep.
		_accumulated += elapsed; 
		_delta = 1.0;
		
		while (_accumulated >= _target_time) {
			required_frames++;
			_accumulated -= _target_time;
		}
	}
	else { // Variable timestep.
		_delta = elapsed / _target_time;
		_accumulated = 0;
		required_frames = 1;
	}

	if (required_frames > 0)
	{
		auto f_time = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time - _prev_tick_time);
		_frame_time = f_time.count();
		_prev_tick_time = time;
	}

	_prev_time = time;
	_frame_id += required_frames;
	return required_frames;
}

