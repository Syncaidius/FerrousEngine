#pragma once
#include "test.hpp"

using namespace std;

class TestMemory : public FerrousTest {
public:

	TestMemory() : FerrousTest("Memory Allocator Test") {}

protected:
	const static int NUM_ALLOCATIONS = 500;

	void onRun(Logger& log) override {
		log.writeLine(FeString::format( "Created allocator with page size: %d bytes", Memory::PAGE_SIZE));

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
		log.writeLine(FeString::format("Randomly deallocated %d blocks"_fe, rngDeallocations));

		//// Output a map of allocator memory.
		log.writeLine(L" "_fe); // TODO empty line overload.
		Memory::get()->outputDebug();
		log.writeLine(L" "_fe);
	}
};