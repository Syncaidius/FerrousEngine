#include "stdafx.h"
#include "logging.h"
#include "strings.h"
#include "localization.h"
#include "game_time.h"
#include "file.h"
#include "stream_file.h"
#include "text_stream_writer.h"
#include "text_stream_reader.h"
#include "shapes.h"
#include <map>
#include <Windows.h>
using namespace std;
using namespace fe;
using namespace fe::shapes;
using namespace glm;


/*	=============================================
	NOTICE: This program is currently used as a scratchpad or testing area 
	for various parts of the engine during development. 

	It will be messy!
	============================================= */

void RunStringTest(Logger* log) {
	Localization* loc = Localization::get();
	loc->setCurrentLocale(loc->getPreferredLocale());
	log->writeLine(L"FeString Test"_fe);
	log->writeLine(L"============="_fe);

	FeString aString("String A"_fe);
	FeString bString(L"String B"_fe);
	FeString cString = " = String C"_fe;
	FeString formattedString = FeString::format("%d plus %d equals %d"_fe, 5, 6, 5 + 6);
	//FeString result = aString + 1 + " + "_fe + bString + 2.3458 + " = The result"_fe;
	//FeString resultLower = result.toLower();
	//FeString resultUpper = result.toUpper();
	FeString resultCapsAll = ("this is a capitalized string"_fe).capitalize();
	FeString resultCapsFirst = ("this is a capitalized string"_fe).capitalizeFirst();

	FeString toTrim = "			   I am trimmed		"_fe;
	FeString resultTrimmed = toTrim.trim();
	FeString resultTrimStart = toTrim.trimStart();
	FeString resultTrimEnd = toTrim.trimEnd();

	FeString toSearch = "I am a string. I can be searched.";
	size_t indexOfString = toSearch.indexOf(&"string"_fe);
	size_t indexOfSearched = toSearch.indexOf(&L"searched"_fe);
	size_t indexOfChicken = toSearch.indexOf(&"chicken"_fe);  

	FeString toReplace = "I am a replaced string full of replacements, which replaces things.";
	FeString resultReplaced = toReplace.replace("replace", "start");
	FeString resultSubStr = toReplace.substr(0, 10);

	bool startsWith = toSearch.startsWith(&"I am"_fe);
	bool startsWithFail = toSearch.startsWith(&"I'm not"_fe);
	bool endsWith = toSearch.endsWith(&"searched."_fe);
	bool endsWithFail = toSearch.endsWith(&"searching!"_fe);

	log->writeLineF("A string: %s", aString.getData());
	log->writeLineF("B string:  %s", bString.getData());
	log->writeLineF("C string:  %s", cString.getData());
	log->writeLineF("Formatted:  %s", formattedString.getData());
	//log->writeLineF("Result:  %s", result.getData());
	log->writeLine(" ");

	//log->writeLineF("Lower-case: %s", resultLower.getData());
	//log->writeLineF("Upper-case: %s",resultUpper.getData());
	log->writeLineF("Capitalized (all): %s",resultCapsAll.getData());
	log->writeLineF("Capitalized (1st): %s",resultCapsFirst.getData());
	log->writeLine(" ");

	log->writeLineF("Trimmed: {%s}",resultTrimmed.getData());
	log->writeLineF("Trimmed (start): {%s}",resultTrimStart.getData());
	log->writeLineF("Trimmed (end): {%s}",resultTrimEnd.getData());
	log->writeLine(" ");

	log->writeLineF("Replacement Target: %s",toReplace.getData());
	log->writeLineF("Replace \"replace\" with \"start\": {%s}",resultReplaced.getData());
	log->writeLineF("Substr 0 to 10: {%s}",resultSubStr.getData());
	log->writeLine(" ");

	log->writeLineF("Target: %s",toSearch.getData());
	log->writeLineF("indexOf(\"string\"): %d",indexOfString);
	log->writeLineF("indexOf(\"searched\"): %d",indexOfSearched);

	log->writeLineF("indexOf(\"chicken\"): %s",(indexOfChicken == FeString::INDEXOF_NONE ? L"not found" : L"found"));
	log->writeLineF("startsWith(\"I am\"): %s",(startsWith ? L"true" : L"false"));
	log->writeLineF("startsWith(\"I'm not\"): %s",(startsWithFail ? L"true" : L"false"));
	log->writeLineF("endsWith(\"searched.\"): %s",(endsWith ? L"true" : L"false"));
	log->writeLineF("endsWith(\"searching!\"): %s", endsWithFail ? L"true" : L"false");
}

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
	Rect32 r1 = Rect32(100,50, 200,150);
	auto center1 = r1.center();
	log->writeLineF("Rect32 -- Left: %d -- Top: %d -- Right: %d -- Bottom: %d -- Width: %d -- Height: %d -- Center: %d,%d",
		r1.left,
		r1.top,
		r1.right,
		r1.bottom,
		r1.width(),
		r1.height(),
		center1.x,
		center1.y);

	RectF r2 = RectF(100.5, 25, 200, 150.1);
	auto center2 = r2.center();
	log->writeLineF("RectF -- Left: %f -- Top: %f -- Right: %f -- Bottom: %f -- Width: %f -- Height: %f -- Center: %f,%f",
		r2.left,
		r2.top,
		r2.right,
		r2.bottom,
		r2.width(),
		r2.height(),
		center2.x,
		center2.y);

	TriangleF t1 = TriangleF(vec2(5.0f, 5.0f), vec2(10.0f, 10.0f), vec2(15.0f, -15.0f));
	auto area1 = t1.area();
	log->writeLineF("TriangleF -- p1: %f,%f -- p2: %f,%f -- p3: %f,%f -- area: %f", t1.p1.x, t1.p1.y, t1.p2.x, t1.p2.y, t1.p3.x, t1.p3.y, area1);

	Triangle32 t2 = Triangle32(ivec2(5, 5), ivec2(10, 10), ivec2(15, 10));
	auto area2 = t2.area();
	log->writeLineF("Triangle32 -- p1: %d,%d -- p2: %d,%d -- p3: %d,%d -- area: %d", t2.p1.x, t2.p1.y, t2.p2.x, t2.p2.y, t2.p3.x, t2.p3.y, area2);
}

const int NUM_ALLOCATIONS = 500;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	Logger log(1);
	ConsoleLogOutput* consoleOutput = new ConsoleLogOutput();
	log.addOutput(consoleOutput);

	log.writeLine(L"Memory allocator test");
	log.writeLine(L"=====================");
	log.writeLineF("Created allocator with page size: %d bytes", Memory::PAGE_SIZE);

	Memory::get()->outputDebug();
	cout << endl;

	//// Do some random allocations/releases
	void* markers[NUM_ALLOCATIONS] = {};
	srand(14);

	//log.writeLineF("Performing %d random allocations..."_fe, NUM_ALLOCATIONS);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t alloc_size = ((uint64_t)rand() % 64) + 1;
		markers[i] = Memory::get()->alloc(alloc_size, 1);
		if (markers[i] != nullptr)
			cout << "Allocated block of " << alloc_size << " bytes at " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
	}

	// Release random blocks
	//log.writeLine("Randomly deallocating..."_fe);
	size_t rngDeallocations = 0;
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t rand_release = (((uint64_t)rand() % 4000) + 1);
		if (rand_release < 2500) {
			rngDeallocations++;
			Memory::get()->dealloc(markers[i]);
			cout << "Deallocating block at " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
			markers[i] = nullptr;
		}
		else {
			cout << "Keeping block at " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
		}
	}
	log.writeLineF("Randomly deallocated %d blocks"_fe, rngDeallocations);

	//// Output a map of allocator memory.
	log.writeLine(L" "_fe); // TODO empty line overload.
	Memory::get()->outputDebug();
	log.writeLine(L" "_fe);

	log.writeLine("Press any key to run string test..."_fe, Color::yellow);
	cin.get();
	RunStringTest(&log);
	log.writeLine(L" "_fe);

	log.writeLine(L" "_fe); // TODO empty line overload.
	/*log.writeLine(L"MEMORY AFTER STRING TEST");*/
	Memory::get()->outputDebug();

	uint32_t defrag_iterations = 1;
	Memory::get()->defragment(1);
	cout << endl;
	cout << "AFTER DEFRAGMENTATION" << endl;
	Memory::get()->outputDebug();

	log.writeLine(L"Press any key to start Rectangle test.", Color::yellow);
	cin.get();
	ShapeTest(&log);

	log.writeLine(L"Press any key to start file I/O test."_fe, Color::yellow);
	cin.get();

	//RunEngineTest(&log);
	FeString workingDir = File::getWorkingDirectory();
	log.writeLineF("Current Path: %s"_fe, workingDir.getData());
	log.writeLineF("   Is directory: %d"_fe, File::isDirectory(workingDir));
	log.writeLineF("   Is file: %d"_fe, File::isFile(&workingDir));

	bool fExists = File::exists(L"FerrousTesting.exe");
	log.writeLineF("File \"FerrousTesting.exe\" found: %d"_fe, fExists);

	FileStream testFileOut = FileStream("test_file.txt"_fe, FileStreamFlags::Create, false, true);
	TextStreamWriter testWriter = TextStreamWriter(&testFileOut, Memory::get());
	log.writeLineF("File created successfully!"_fe);
	testWriter.writeLine(U"This is a test file! 这是一个测试文件! Это тестовый файл! これはテストファイルです!"_fe);

	size_t fSize = testFileOut.getSize();
	testFileOut.close();

	log.writeLineF("Written %d bytes to file"_fe, fSize);
	log.writeLine("File closed"_fe, Color::green);

	// Now open to read
	FileStream testFileIn = FileStream("test_file.txt"_fe, FileStreamFlags::None, true, false);
	TextStreamReader testReader = TextStreamReader(&testFileIn, Memory::get());

	FeString stringFromFile = L"\0"_fe;
	fSize = testFileIn.getSize();

	log.writeLineF("Open file with size: %d bytes"_fe, Color::limeGreen, fSize);
	testReader.readLine(&stringFromFile);
	testFileIn.close();

	log.writeLine("File closed"_fe, Color::red);
	log.writeLineF("String read from file: %s"_fe, Color::darkRed, stringFromFile.getData());

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