#include "stdafx.h";
#include "allocation.h";

/* Allocates a memory stack with with the specified number of bytes, in a single contiguous block.*/
StackAllocator* Allocator::allocStack(const size_t num_bytes, const uint8_t alignment) {
	size_t stack_header = sizeof(StackAllocator);
	size_t total_bytes = num_bytes + stack_header + alignment;
	void* p = alloc(total_bytes);
	void* pAligned = p;
	uint8_t adjustment = align(pAligned, alignment, stack_header);
	return new (p) StackAllocator(this, pAligned, num_bytes);
}

uint8_t Allocator::align(void*& p, uint8_t alignment, size_t start_offset) {
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