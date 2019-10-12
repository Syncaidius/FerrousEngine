#include "stdafx.h"
#include "logging.h"
#include "strings.h"
#include "game_time.h"
#include <map>
#include <Windows.h>

#include "ferrous_test.hpp"
#include "test_memory.hpp"
#include "test_strings.hpp"
#include "test_shapes.hpp"
#include "test_io.hpp"

using namespace std;
using namespace fe;
using namespace fe::shapes;
using namespace glm;


/*	=============================================
	NOTICE: This program is currently used as a scratchpad or testing area 
	for various parts of the engine during development. 

	It will be messy!
	============================================= */

void RunEngineTest(Logger* log) {
	GameTime* timer = new GameTime(true, 60);
	while (true) {
		uint32_t updates_needed = timer->tick();

		for (int i = 0; i < updates_needed; i++) {
			log->writeLineF("Frame %d -- time: %f ms -- delta: %f"_fe, timer->getFrameId(), timer->getFrameTime(), timer->getDelta());
		}
	}
}

void ShapeTest(Logger* log) {

}



template<typename T>
void createAndRunTest(Logger& log) {
	static_assert(std::is_base_of<FerrousTest, T>::value);
	T test = T();
	test.run(log);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	Logger log(1);
	ConsoleLogOutput* consoleOutput = new ConsoleLogOutput();
	log.addOutput(consoleOutput);

	createAndRunTest<TestMemory>(log);
	createAndRunTest<TestStrings>(log);
	createAndRunTest<TestShapes>(log);
	createAndRunTest<TestIO>(log);

	log.writeLine(L"Press any key to start file I/O test."_fe, Color::yellow);
	cin.get();

	//RunEngineTest(&log);
	

	//GameTime test = GameTime(false, 60);
	//double timeStart = test.getTotalTime();

	//int iterations = 5000;
	//for (int i = 0; i < iterations; i++) {
	//	test.tick();
	//	log.writeLine("test");
	//}

	//// [16:04:47] Time taken to log 10000 lines: 12701.788800 ms - std::endl
	//double timeTaken = test.getTotalTime() - timeStart;
	//log.writeLineF(L"Time taken to log %d lines: %f ms", iterations, timeTaken);
	log.writeLine(L"Press any key to exit..."_fe, Color::yellow);
	cin.get();

	return 0;
}