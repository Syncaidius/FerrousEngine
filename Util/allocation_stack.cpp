#include "allocation.h";

StackAllocator::StackAllocator(Allocator* parent, void* mem) {
	_parent = parent;

	_mem = mem;
	_pos = _mem;
}

StackAllocator::~StackAllocator(void) {
	_parent->dealloc(this);
}

void* StackAllocator::alloc(size_t num_bytes) {
	void* p = _pos;
	_pos = (void*)((char*)_pos + num_bytes);
	// TODO ensure stack cannot overflow into memory it does not own when full.
	return _pos;
}

void StackAllocator::revertTo(void* p) {
	_pos = p;
}

void StackAllocator::reset() {
	_pos = (void*)((char*)_mem);
}

size_t StackAllocator::getTotalAllocated() {
	return &_pos - &_mem;
}

void StackAllocator::dealloc(void* p) {
	// TODO implement (track correctly so we can automatically revertTo() when appropriate.
}