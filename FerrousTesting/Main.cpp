#include "stdafx.h";
#include <Util/util.h>;
#include <Util/logging.h>
#include <Util/string_fe.h>
#include <Util/localization.h>
#include <map>
#include <Windows.h>
using namespace std;

void OutputFreeList(Logger* log) {
	Memory* mem = Memory::get();
	size_t overhead = mem->getOverhead();
	size_t allocated = mem->getTotalAllocated();
	size_t total = allocated;
	size_t tracked_total = allocated + overhead;
	size_t num_free_blocks = 0;
	size_t capacity = mem->getPageCount() * Memory::PAGE_SIZE;
	size_t free = capacity - tracked_total;

	double overhead_percent = ((double)overhead / (double)capacity) * 100.0;
	double tracked_total_percent = ((double)tracked_total / (double)capacity) * 100.0;
	double free_percent = ((double)free / (double)capacity) * 100.0;

	log->writeLineF("Allocated: %d/%d bytes", allocated, capacity);
	log->writeLineF("Overhead: %d bytes(%f%%)", overhead, overhead_percent);
	log->writeLineF("Free: %d/%d bytes(%f%%)", free, capacity, free_percent);
	log->writeLineF("Pages: %d", mem->getPageCount());
	log->writeLineF("Tracked total: %d/%d bytes(%f%%)", tracked_total, capacity, tracked_total_percent);

	Memory::Page* page = mem->getFirstPage();
	int page_id = 0;
	while (page != nullptr) {
		log->writeLineF("PAGE %d START[", page_id);

		Memory::Block* freeList = page->getFreeList();
		while (freeList != nullptr) {
			uintptr_t p = reinterpret_cast<uintptr_t>(freeList);
			uintptr_t p_next = reinterpret_cast<uintptr_t>(freeList->getNext());
			uintptr_t p_end = reinterpret_cast<uint64_t>(freeList) + Memory::BLOCK_HEADER_SIZE + freeList->getSize();
			log->writeLineF("  ptr: %d -- end: %d -- size: %d -- Next ptr: %d", p, p_end, freeList->getSize(), p_next);
			total += freeList->getSize();

			//if (freeList->getNext() != nullptr) {
			//	if (p_next >= p)
			//		cout << "      diff to next: " << (p_next - p) << "bytes" << endl;
			//	else
			//		cout << "      diff to next: -" << (p - p_next) << " bytes" << endl;
			//}
			freeList = freeList->getNext();
			num_free_blocks++;
		}

		log->writeLineF("] PAGE %d END", page_id);
		page = page->getNext();
		page_id++;
	}

	cout << "Total: " << total << "/" << capacity << " -- Free blocks: " << num_free_blocks << endl;
	if (total < capacity)
		cout << "LEAK DETECTED: " << (capacity - total) << " bytes" << endl;
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
	log->writeLineF(" ");

	log->writeLineF("Lower-case: %s", resultLower.c_str());
	log->writeLineF("Upper-case: %s",resultUpper.c_str());
	log->writeLineF("Capitalized (all): %s",resultCapsAll.c_str());
	log->writeLineF("Capitalized (1st): %s",resultCapsFirst.c_str());
	log->writeLineF(" ");

	log->writeLineF("Trimmed: {%s}",resultTrimmed.c_str());
	log->writeLineF("Trimmed (start): {%s}",resultTrimStart.c_str());
	log->writeLineF("Trimmed (end): {%s}",resultTrimEnd.c_str());
	log->writeLineF(" ");

	log->writeLineF("Replacement Target: %s",toReplace.c_str());
	log->writeLineF("Replace \"replace\" with \"start\": {%s}",resultReplaced.c_str());
	log->writeLineF("Substr 0 to 10: {%s}",resultSubStr.c_str());
	log->writeLineF(" ");

	log->writeLineF("Target: %s",toSearch.c_str());
	log->writeLineF("indexOf(\"string\"): %d",indexOfString);
	log->writeLineF("indexOf(\"searched\"): %d",indexOfSearched);
	log->writeLineF("indexOf(\"chicken\"): %s",(indexOfChicken == FeString::INDEXOF_NONE ? "not found" : "found"));
	log->writeLineF("startsWith(\"I am\"): %s",(startsWith ? "true" : "false"));
	log->writeLineF("startsWith(\"I'm not\"): %s",(startsWithFail ? "true" : "false"));
	log->writeLineF("endsWith(\"searched.\"): %s",(endsWith ? "true" : "false"));
	log->writeLineF("endsWith(\"searching!\"): %s", endsWithFail ? "true" : "false");
}

const int NUM_ALLOCATIONS = 40;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	Logger log(1);
	LogConsoleOutput* consoleOutput = new LogConsoleOutput();
	log.addOutput(consoleOutput);

	log.writeLine(L"Memory allocator test");
	log.writeLine(L"=====================");
	log.writeLineF("Created allocator with page size: %d bytes"_fe, Memory::PAGE_SIZE);

	OutputFreeList(&log);
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
			cout << "Did not release block " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
		}
	}

	// Output a map of allocator memory.
	//SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	log.writeLine(L" "); // TODO empty line overload.
	log.writeLine(L"BEFORE STRING TEST");
	OutputFreeList(&log);
	log.writeLine(L" ");

	log.writeLine("Press any key to run string test...");
	cin.get();
	RunStringTest(&log);
	log.writeLine(L" ");

	log.writeLine(L" "); // TODO empty line overload.
	log.writeLine(L"AFTER STRING TEST");
	OutputFreeList(&log);

	uint32_t defrag_iterations = 1;
	Memory::get()->defragment(1);
	cout << endl;
	cout << "AFTER DEFRAGMENTATION" << endl;
	OutputFreeList(&log);

	log.writeLine("Press any key to exit...");
	cin.get();
	return 0;
}