#include "stdafx.h";
#include "allocation.h";

/* Allocates a memory stack with with the specified number of bytes, in a single contiguous block.*/
StackAllocator* Allocator::allocStack(const size_t size_bytes, const uint8_t alignment) {
	size_t stack_header = sizeof(StackAllocator);
	size_t total_bytes = size_bytes + stack_header + alignment;
	void* p = alloc(total_bytes);
	void* alignedAddr = p; // align(p, alignment, stack_header);
	return new (p) StackAllocator(this, alignedAddr);
}


