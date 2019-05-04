#include "stdafx.h";
#include <Util/util.h>;
#include <Util/logging.h>
#include <map>
#include <Windows.h>

using namespace std;
static const WORD MAX_CONSOLE_LINES = 500;


void OutputFreeList(Logger* log) {
	MemoryAllocator* mem = MemoryAllocator::get();
	uint64_t overhead = mem->getOverhead();
	uint64_t allocated = mem->getAllocated();
	uint64_t total = allocated;
	uint64_t num_free = 0;

	uint64_t size = MemoryAllocator::PAGE_SIZE; // TODO replace with getSize() whenever paging is implemented.
	double overhead_percent = ((double)overhead / (double)size) * 100.0;

	cout << "   Allocated: " << allocated  << "/" << size << " bytes -- Overhead: " << overhead << " bytes (" << overhead_percent << "%)" << endl;

	MemoryAllocator::FreeBlock* freeList = mem->getFreeList();
	while (freeList != nullptr) {
		uintptr_t p = reinterpret_cast<uintptr_t>(freeList);
		uintptr_t p_next = reinterpret_cast<uintptr_t>(freeList->next);
		uint64_t p_end = reinterpret_cast<uint64_t>(freeList) + MemoryAllocator::HEADER_SIZE + freeList->size;
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

void CreateTestObject() {
	cout << "Spawning test object" << endl;
	std::map<string, int> test_map;
}

const int NUM_ALLOCATIONS = 40;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	Logger log(1);
	LogConsoleOutput* consoleOutput = new LogConsoleOutput();
	log.addOutput(consoleOutput);

	log.writeLine(L"Memory allocator test");
	log.writeLine(L"=====================");
	cout << "Created allocator with page size: " << MemoryAllocator::PAGE_SIZE << " bytes" << endl;
	CreateTestObject();
	OutputFreeList(&log);
	cout << endl;

	// Do some random allocations/releases
	void* markers[NUM_ALLOCATIONS] = {};
	srand(14);

	//SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t alloc_size = ((uint64_t)rand() % 64) + 1;
		markers[i] = MemoryAllocator::get()->alloc(alloc_size);
		if(markers[i] != nullptr) 
			cout << "Allocated block of " << alloc_size << " bytes at " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
	}

	// Release random blocks
	//SetConsoleTextAttribute(console, FOREGROUND_INTENSITY);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t rand_release = (((uint64_t)rand() % 4000) + 1);
		if (rand_release < 2500) {
			MemoryAllocator::get()->dealloc(markers[i]);
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
	MemoryAllocator::get()->defragment();
	cout << endl;
	cout << "AFTER " << defrag_iterations << " DEFRAGMENTATION ITERATIONS" << endl;
	OutputFreeList(&log);
	cin.get();
	return 0;
}