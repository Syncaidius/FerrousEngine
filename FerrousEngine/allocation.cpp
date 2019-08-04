#include "stdafx.h";
#include "allocation.h";

uint8_t FerrousAllocator::alignForwardAdjustment(const void* p, uint8_t alignment) {
	assert(alignment >= 1);
	assert(alignment <= 128);
	assert((alignment & (alignment - 1)) == 0);


	uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(p) & static_cast<uintptr_t>(alignment - 1));
	if (adjustment == alignment)
		return 0;

	return adjustment;
}

void* FerrousAllocator::alignForward(const void* p, uint8_t alignment) {
	uintptr_t mask = alignment - 1;
	if ((alignment > 1) && (((uintptr_t)p & mask) != 0))
		return reinterpret_cast<void*>((uintptr_t)p + ((uintptr_t)alignment - ((uintptr_t)p & mask)));
	else
		return const_cast<void*>(p);

	// Original
	//return reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(p)) + ((alignment - 1) & (~(alignment - 1))));
}