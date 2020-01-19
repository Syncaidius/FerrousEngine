#pragma once
#include "stdafx.h"
#include "test.hpp"
#include "game_time.h"

class TestEngine : public FerrousTest {
public:

	TestEngine() : FerrousTest("Engine Test") {}

protected:
	void onRun(Logger& log) override {
		GameTime* timer = new GameTime(true, 60);
		while (true) {
			uint32_t updates_needed = timer->tick();

			for (int i = 0; i < updates_needed; i++) {
				log.writeLine(FeString::format("Frame %d -- time: %f ms -- delta: %f"_fe, timer->getFrameId(), timer->getFrameTime(), timer->getDelta()));
			}
		}
	}
};