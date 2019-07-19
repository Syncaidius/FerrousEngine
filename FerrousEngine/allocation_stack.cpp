#include "allocation_stack.h";

StackAllocator::StackAllocator(FerrousAllocator* parent, size_t numBytes) {
	_parent = parent;
	_mem = _parent->alloc(numBytes);
	void* pAligned = _mem;

	_pos = _mem;
	_capacity = numBytes;
}

StackAllocator::~StackAllocator(void) {
	_parent->dealloc(_mem);
	_parent = nullptr;
}

void* StackAllocator::alloc(size_t num_bytes) {
	void* p = _pos;
	_pos = (void*)((char*)_pos + num_bytes);
	// TODO ensure stack cannot overflow into memory it does not own when full.
	return _pos;
}

void* StackAllocator::allocAligned(const size_t sizeBytes, const uint8_t alignment) {
	size_t expanded_bytes = sizeBytes + alignment;
	void* p = alloc(expanded_bytes);
	size_t adjustment = align(p, alignment, 0);
	return p;
}

void StackAllocator::revertTo(void* p) {
	_pos = p;
}

void StackAllocator::reset() {
	_pos = (void*)((char*)_mem);
}

size_t StackAllocator::getUsed() {
	return &_pos - &_mem;
}

size_t StackAllocator::getCapacity() {
	return _capacity;
}

void StackAllocator::dealloc(void* p) {
	// Do nothing. Stack allocator relies solely on reset() or revertTo() for reusing memory.
}

void StackAllocator::ref(const void* p) {
	// Do nothing. Stack allocator does not reference-count.
}

void StackAllocator::deref(void* p) {
	// Do nothing. Stack allocator does not reference-count.
}