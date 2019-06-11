#include "stdafx.h";
#include <Util/util.h>;
#include <Util/logging.h>
#include <Util/string_fe.h>
#include <Util/localization.h>
#include <Util/game_time.h>
#include <Util/file.h>
#include <map>
#include <Windows.h>
using namespace std;

/*	=============================================
	NOTICE: This program is currently used as a scratchpad or testing area 
	for various parts of the engine during development. 

	It will be messy!
	============================================= */

void OutputFreeList() {
	//NOTE: we use cout here to avoid interfering with memory allocation statistics.

	Memory* mem = Memory::get();

	// Track reported stats
	Memory::Page* p = mem->getFirstPage();
	size_t r_pages = mem->getPageCount();
	size_t a_pages = 0;

	while (p) {
		// Reported counts
		size_t pr_alloc = p->getAllocated();
		size_t pr_overhead = p->getOverhead();
		size_t pr_free = Memory::PAGE_SIZE - pr_alloc;
		size_t pr_blocks_alloc = p->getBlocksAllocated();
		size_t pr_blocks_free = p->getBlocksFree();

		// Actual counters
		size_t pa_alloc = 0;
		size_t pa_overhead = 0;
		size_t pa_free = 0;
		size_t pa_blocks_alloc = 0;
		size_t pa_blocks_free = 0;

		cout << "PAGE " << (a_pages + 1) << " OF " << r_pages << endl;
		cout << "   Blocks:" << endl;

		// Iterate through all blocks to calculate exactly how much overhead there is
		char* page_data = reinterpret_cast<char*>(p) + Memory::PAGE_HEADER_SIZE;
		char* data_end = reinterpret_cast<char*>(p) + Memory::PAGE_SIZE;

		Memory::Block* b = nullptr;
		while (page_data < data_end) {
			b = reinterpret_cast<Memory::Block*>(page_data);
			pa_blocks_alloc++;
			page_data += Memory::BLOCK_HEADER_SIZE + b->getSize();
		}

		// Calculate free block size
		b = p->getFreeList();
		while (b) {
			pa_free += b->getSize();		
			pa_blocks_free++;

			cout << "      (" << pa_blocks_free << ") Address: " << b << " -- size: " << b->getSize() << " bytes" << endl;
			b = b->getNext();
		}

		pa_overhead = (pa_blocks_alloc * Memory::BLOCK_HEADER_SIZE);
		pa_overhead += Memory::PAGE_HEADER_SIZE;
		pa_blocks_alloc -= pa_blocks_free;
		pa_alloc = Memory::PAGE_SIZE - pa_free;

		cout << endl;
		cout << "   Reported:" << endl;
		cout << "      -- Allocated: " << pr_alloc << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << "      -- Overhead: " << pr_overhead << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << "      -- Free: " << pr_free << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << "      -- Allocated Blocks: " << pr_blocks_alloc << endl;
		cout << "      -- Free Blocks: " << pr_blocks_free << endl;
		cout << "      -- Alloc & Free Total: " << (pr_free + pr_alloc) << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << endl;
		cout << "   Actual:" << endl;
		cout << "      -- Allocated: " << pa_alloc << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << "      -- Overhead: " << pa_overhead << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << "      -- Free: " << pa_free << "/" << Memory::PAGE_SIZE << " bytes" << endl;
		cout << "      -- Allocated Blocks: " << pa_blocks_alloc << endl;
		cout << "      -- Free Blocks: " << pa_blocks_free << endl;
		cout << "      -- Alloc & Free Total: " << (pa_free + pa_alloc) << "/" << Memory::PAGE_SIZE << " bytes" << endl;

		p = p->getNext();
		a_pages++;
	}
}

void RunStringTest(Logger* log) {
	Localization* loc = Localization::get();
	loc->setCurrentLocale(loc->getPreferredLocale());
	log->writeLine(L"FeString Test");
	log->writeLine(L"=============");

	FeString aString("String A");
	FeString bString(L"String B");
	FeString cString = " = String C"_fe;
	FeString formattedString = FeString::format("%d plus %d equals %d"_fe, 5, 6, 5 + 6);
	FeString result = aString + 1 + " + " + bString + 2.3458 + " = The result"_fe;
	FeString resultLower = result.toLower();
	FeString resultUpper = result.toUpper();
	FeString resultCapsAll = ("this is a capitalized string"_fe).capitalize();
	FeString resultCapsFirst = ("this is a capitalized string"_fe).capitalizeFirst();

	FeString toTrim = "			   I am trimmed		"_fe;
	FeString resultTrimmed = toTrim.trim();
	FeString resultTrimStart = toTrim.trimStart();
	FeString resultTrimEnd = toTrim.trimEnd();

	FeString toSearch = "I am a string. I can be searched.";
	size_t indexOfString = toSearch.indexOf(&"string"_fe);
	size_t indexOfSearched = toSearch.indexOf(&"searched"_fe);
	size_t indexOfChicken = toSearch.indexOf(&"chicken"_fe);

	FeString toReplace = "I am a replaced string full of replacements, which replaces things.";
	FeString resultReplaced = toReplace.replace(&"replace"_fe, &"start"_fe);
	FeString resultSubStr = toReplace.substr(0, 10);

	bool startsWith = toSearch.startsWith(&"I am"_fe);
	bool startsWithFail = toSearch.startsWith(&"I'm not"_fe);
	bool endsWith = toSearch.endsWith(&"searched."_fe);
	bool endsWithFail = toSearch.endsWith(&"searching!"_fe);

	log->writeLineF("A string: %s", aString.c_str());
	log->writeLineF("B string:  %s", bString.c_str());
	log->writeLineF("C string:  %s", cString.c_str());
	log->writeLineF("Formatted:  %s", formattedString.c_str());
	log->writeLineF("Result:  %s", result.c_str());
	log->writeLine(" ");

	log->writeLineF("Lower-case: %s", resultLower.c_str());
	log->writeLineF("Upper-case: %s",resultUpper.c_str());
	log->writeLineF("Capitalized (all): %s",resultCapsAll.c_str());
	log->writeLineF("Capitalized (1st): %s",resultCapsFirst.c_str());
	log->writeLine(" ");

	log->writeLineF("Trimmed: {%s}",resultTrimmed.c_str());
	log->writeLineF("Trimmed (start): {%s}",resultTrimStart.c_str());
	log->writeLineF("Trimmed (end): {%s}",resultTrimEnd.c_str());
	log->writeLine(" ");

	log->writeLineF("Replacement Target: %s",toReplace.c_str());
	log->writeLineF("Replace \"replace\" with \"start\": {%s}",resultReplaced.c_str());
	log->writeLineF("Substr 0 to 10: {%s}",resultSubStr.c_str());
	log->writeLine(" ");

	log->writeLineF("Target: %s",toSearch.c_str());
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
			log->writeLineF("Frame %d -- time: %f ms -- delta: %f", timer->getFrameId(), timer->getFrameTime(), timer->getDelta());
		}
	}
}

const int NUM_ALLOCATIONS = 40;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	Logger log(1);
	LogConsoleOutput* consoleOutput = new LogConsoleOutput();
	log.addOutput(consoleOutput);

	log.writeLine(L"Memory allocator test");
	log.writeLine(L"=====================");
	log.writeLineF("Created allocator with page size: %d bytes"_fe, Memory::PAGE_SIZE);

	OutputFreeList();
	cout << endl;

	// Do some random allocations/releases
	void* markers[NUM_ALLOCATIONS] = {};
	srand(14);

	//SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t alloc_size = ((uint64_t)rand() % 64) + 1;
		markers[i] = Memory::get()->alloc(alloc_size);
		if (markers[i] != nullptr)
			cout << "Allocated block of " << alloc_size << " bytes at " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
	}

	// Release random blocks
	//SetConsoleTextAttribute(console, FOREGROUND_INTENSITY);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t rand_release = (((uint64_t)rand() % 4000) + 1);
		if (rand_release < 2500) {
			Memory::get()->dealloc(markers[i]);
			cout << "Released block " << reinterpret_cast<uintptr_t>(markers[i]) << " via allocator" << endl;
			markers[i] = nullptr;
		}
		else {
			cout << "Kept block " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
		}
	}

	// Output a map of allocator memory.
	//SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	//log.writeLine(L" "); // TODO empty line overload.
	//OutputFreeList();
	//log.writeLine(L" ");

	//log.writeLine("Press any key to run string test...");
	//cin.get();
	//RunStringTest(&log);
	//log.writeLine(L" ");

	//log.writeLine(L" "); // TODO empty line overload.
	//log.writeLine(L"MEMORY AFTER STRING TEST");
	//OutputFreeList();

	//uint32_t defrag_iterations = 1;
	//Memory::get()->defragment(1);
	//cout << endl;
	//cout << "AFTER DEFRAGMENTATION" << endl;
	//OutputFreeList();

	//log.writeLine(L"Press any key to exit...");
	//cin.get();

	//RunEngineTest(&log);
	FeString workingDir = File::getWorkingDirectory();
	log.writeLineF("Current Path: %s", workingDir.c_str());
	log.writeLineF("   Is directory: %d", File::isDirectory(&workingDir));
	log.writeLineF("   Is file: %d", File::isFile(&workingDir));

	bool fExists = File::exists(L"FerrousTesting.exe");
	log.writeLineF("File \"FerrousTesting.exe\" found: %d", fExists);

	File* testFile;
	File::Result r = File::open(L"test_file.txt", testFile, File::AccessFlags::Write, File::ModeFlags::Create);
	log.writeLineF("File::open() result: %d", r);
	if (r == File::Result::Success) {
		log.writeLineF("File created successfully!");
	}
	testFile->close();


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
	cin.get();

	return 0;
}