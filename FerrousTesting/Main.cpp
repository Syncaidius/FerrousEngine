#include "stdafx.h";
#include <Util/MemoryAllocator.h>;
#include <cstdlib>;

using namespace std;
static const WORD MAX_CONSOLE_LINES = 500;

void OutputFreeList(MemoryAllocator* allocator) {
	uint64_t overhead = allocator->getOverhead();
	uint64_t size = allocator->getSize();
	double overhead_percent = ((double)overhead / (double)size) * 100.0;
	cout << "   Allocated: " << allocator->getAllocated() << "/" << size << " bytes -- Overhead: " << overhead << " bytes (" << overhead_percent << "%)" << endl;

	MemoryAllocator::FreeBlock* freeList = allocator->getFreeList();
	while (freeList != nullptr) {
		uintptr_t p = reinterpret_cast<uintptr_t>(freeList);
		uintptr_t p_next = reinterpret_cast<uintptr_t>(freeList->next);
		uint64_t p_end = reinterpret_cast<uint64_t>(freeList) + MemoryAllocator::HEADER_SIZE + freeList->size;
		cout << "   ptr: " << p << " -- end: " << p_end << " -- size: " << freeList->size << " -- Next ptr: " << p_next << endl;

		if (freeList->next != nullptr) {
			if (p_next >= p)
				cout << "      diff to next: " << (p_next - p) << "bytes" << endl;
			else
				cout << "      diff to next: -" << (p - p_next) << " bytes" << endl;
		}
		freeList = freeList->next;
	}
}

void RedirectToConsole() {
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE* fp;

	FILE* pNewStdout = nullptr;
	FILE* pNewStderr = nullptr;
	FILE* pNewStdin = nullptr;

	// allocate a console for this app
	AllocConsole();
	freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
	freopen_s(&pNewStdin, "CONIN$", "r", stdin);

	// Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
	// to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
	// which seems to occur in newer version of Visual Studio even when the console has not been read from or written
	// to yet.
	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();

	std::wcout.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

const int NUM_ALLOCATIONS = 50;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
	RedirectToConsole();
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	cout << "Memory allocator test" << endl;
	cout << "=====================" << endl;
	uint64_t alloc_bytes = 8192;
	MemoryAllocator allocator(alloc_bytes);
	cout << "Created allocator with size: " << alloc_bytes << " bytes" << endl;
	OutputFreeList(&allocator);
	cout << endl;

	// Do some random allocations/releases
	MemoryAllocator::Marker markers[NUM_ALLOCATIONS] = {};
	srand(14);

	SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t alloc_size = ((uint64_t)rand() % 64) + 1;
		markers[i] = allocator.alloc(alloc_size);
		if(markers[i] != nullptr)
			cout << "Allocated block of " << alloc_size << " bytes at " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
	}

	// Release random blocks
	SetConsoleTextAttribute(console, FOREGROUND_INTENSITY);
	for (int i = 0; i < NUM_ALLOCATIONS; i++) {
		uint64_t rand_release = (((uint64_t)rand() % 4000) + 1);
		if (rand_release < 2500) {
			allocator.release(markers[i]);
			cout << "Released block " << reinterpret_cast<uintptr_t>(markers[i]) << " via allocator" << endl;
			markers[i] = nullptr;
		}
		else {
			cout << "Did not release block " << reinterpret_cast<uintptr_t>(markers[i]) << endl;
		}
	}

	// Output a map of allocator memory.
	SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << endl;
	cout << "BEFORE DEFRAGMENTATION" << endl;
	OutputFreeList(&allocator);

	uint32_t defrag_iterations = 1;
	allocator.defragment(defrag_iterations);
	cout << endl;
	cout << "AFTER " << defrag_iterations << " DEFRAGMENTATION ITERATIONS" << endl;
	OutputFreeList(&allocator);
	cin.get();
	FreeConsole();
	return 0;
}