#pragma once
#include "stdafx.h"
#include <stdint.h>
#include <chrono>
class GameTime{
public:
	GameTime(bool fixedTimestep = true, uint32_t targetFps = 60);
	~GameTime();

	bool getPaused();
	void setPaused(bool paused);

	uint32_t getTargetFps();
	void setTargetFps(uint32_t fps);
	
	bool getFixedTimestep();
	void setFixedTimestep(bool fixed);

	uint32_t getFps();
	double getFrameTime();
	double getTargetFrameTime();

	/* The total time recorded by the current GameTime instance. */
	double getTotalTime();

	uint64_t getFrameId();

	double getDelta();

	/* Updates the current game time and returns the number of updates required to advance. */
	uint32_t tick();
private:
	bool _is_fixed;
	bool _paused;
	uint64_t _frame_id;
	uint32_t _target_fps;
	uint32_t _fps;
	double _frame_time; /* The time of the previously-completed tick() */
	double _total_time;
	double _target_time;
	double _delta;
	double _accumulated;

	std::chrono::high_resolution_clock::time_point _prev_time;
	std::chrono::high_resolution_clock::time_point _prev_tick_time;
};