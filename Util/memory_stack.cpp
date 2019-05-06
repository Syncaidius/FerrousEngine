#include "memory.h";

Memory::Stack::Stack(Memory* mem_parent, void* mem) {
	_parent = mem_parent;

	_start = mem;
	_pos = _start;
}

Memory::Stack::~Stack(void) {
	_parent->dealloc(this);
}

void* Memory::Stack::alloc(size_t num_bytes) {
	void* p = _pos;
	_pos = (void*)((char*)_pos + num_bytes);
	// TODO ensure stack cannot overflow into memory it does not own when full.
	return _pos;
}

void Memory::Stack::revertTo(void* p) {
	_pos = p;
}

void Memory::Stack::reset() {
	_pos = (void*)((char*)_start);
}