#pragma once
#include "util.h"
#include <stdint.h>
#include <chrono>

class GameTime{
public:
	GameTime(bool fixedTimestep, uint32_t targetFps);
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

	/* The total amount of frame time recorded by this timer, across all frames. */
	double getTotalTime();

	/* Updates the current game time and returns the number of updates required to advance. */
	uint32_t tick();
private:
	bool _is_fixed;
	bool _paused;
	uint64_t _total_frames;
	uint32_t _target_fps;
	uint32_t _fps;
	double _frame_time; /* The time of the previous frame */
	double _total_time;
	double _target_time;
	double _delta;
	double _accumulated;

	std::chrono::high_resolution_clock::time_point _prev_time;
};