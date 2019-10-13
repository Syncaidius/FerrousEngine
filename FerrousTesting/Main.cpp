#include "stdafx.h"
#include "logging.h"
#include "strings.h"
#include <map>
#include <Windows.h>

#include "ferrous_test.hpp"
#include "test_memory.hpp"
#include "test_strings.hpp"
#include "test_shapes.hpp"
#include "test_io.hpp"
#include "test_engine.h"

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

	//createAndRunTest<TestEngine>(log);

	log.writeLine(L"Press any key to exit..."_fe, Color::yellow);
	cin.get();

	return 0;
}