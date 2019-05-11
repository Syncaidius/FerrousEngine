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
	size_t allocated = mem->getAllocated();
	size_t total = allocated;
	size_t num_free = 0;

	size_t size = Memory::PAGE_SIZE; // TODO replace with getSize() whenever paging is implemented.
	double overhead_percent = ((double)overhead / (double)size) * 100.0;

	cout << "   Allocated: " << allocated << "/" << size << " bytes -- Overhead: " << overhead << " bytes (" << overhead_percent << "%)" << endl;

	Memory::Block * freeList = mem->getFreeList();
	while (freeList != nullptr) {
		uintptr_t p = reinterpret_cast<uintptr_t>(freeList);
		uintptr_t p_next = reinterpret_cast<uintptr_t>(freeList->next);
		uintptr_t p_end = reinterpret_cast<uint64_t>(freeList) + Memory::HEADER_SIZE + freeList->size;
		cout << "   ptr: " << p << " -- end: " << p_end << " -- size: " << freeList->size << " -- Next ptr: " << p_next << endl;
		total += freeList->size;

		if (freeList->next != nullptr) {
			if (p_next >= p)
				cout << "      diff to next: " << (p_next - p) << "bytes" << endl;
			else
				cout << "      diff to next: -" << (p - p_next) << " bytes" << endl;
		}
		freeList = freeList->next;
		num_free++;
	}

	cout << "Total: " << total << "/" << size << " -- Free blocks: " << num_free << endl;
	if (total < size)
		cout << "LEAK DETECTED: " << (size - total) << " bytes" << endl;
}

void RunStringTest(Logger* log) {
	Localization* loc = Localization::get();
	loc->setCurrentLocale(loc->getPreferredLocale());
	log->writeLine(L"FeString Test");
	log->writeLine(L"=============");

	FeString aString("String A");
	FeString bString(L"String B");
	FeString cString = " = String C"_fe;
	FeString result = aString + " + " + bString + " = The result"_fe;
	FeString resultLower = result.toLower();
	FeString resultUpper = result.toUpper();
	FeString resultCapsAll = ("this is a capitalized string"_fe).capitalize();
	FeString resultCapsFirst = ("this is a capitalized string"_fe).capitalizeFirst();

	wcout << "A string: " << aString.c_str() << endl;
	wcout << "B string: " << bString.c_str() << endl;
	wcout << "C string: " << cString.c_str() << endl;
	wcout << "Result: " << result.c_str() << endl;
	wcout << "Lower-case: " << resultLower.c_str() << endl;
	wcout << "Upper-case: " << resultUpper.c_str() << endl;
	wcout << "Capitalized (all): " << resultCapsAll.c_str() << endl;
	wcout << "Capitalized (1st): " << resultCapsFirst.c_str() << endl;
}

const int NUM_ALLOCATIONS = 40;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	Logger log(1);
	LogConsoleOutput* consoleOutput = new LogConsoleOutput();
	log.addOutput(consoleOutput);

	log.writeLine(L"Memory allocator test");
	log.writeLine(L"=====================");
	cout << "Created allocator with page size: " << Memory::PAGE_SIZE << " bytes" << endl;

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
	log.writeLine(L"BEFORE DEFRAGMENTATION");
	OutputFreeList(&log);

	uint32_t defrag_iterations = 1;
	Memory::get()->defragment();
	cout << endl;
	cout << "AFTER " << defrag_iterations << " DEFRAGMENTATION ITERATIONS" << endl;
	OutputFreeList(&log);

	RunStringTest(&log);

	cin.get();
	return 0;
}