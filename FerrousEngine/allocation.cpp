#include "stdafx.h";
#include "allocation.h";

uint8_t FerrousAllocator::align(void*& p, uint8_t alignment, size_t start_offset) {
	assert(alignment >= 1);
	assert(alignment <= 128);
	assert((alignment & (alignment - 1)) == 0);;

	/* Calc adjustment by masking off the lower bits of address, to determine how "misaligned" it is.*/
	size_t mask = (alignment - 1); // TODO Can we allocate after this and save 1 byte of memory per align-alloc?

	uintptr_t raw_start = reinterpret_cast<uintptr_t>(p) + start_offset;
	size_t misalignment = (raw_start & mask);

	uint8_t adjustment = alignment - misalignment;
	uintptr_t alignedAddr = raw_start + adjustment;

	p = reinterpret_cast<void*>(alignedAddr);
	return adjustment;
}