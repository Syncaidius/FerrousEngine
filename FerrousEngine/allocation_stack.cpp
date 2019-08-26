#include "allocation_stack.h";

namespace fe {
	StackAllocator::StackAllocator(FerrousAllocator* parent, size_t numBytes) {
		_parent = parent;
		_mem = _parent->alloc(numBytes, 1);

		_pos = _mem;
		_capacity = numBytes;
	}

	StackAllocator::~StackAllocator(void) {
		_parent->dealloc(_mem);
		_parent = nullptr;
	}

	void* StackAllocator::alloc(size_t num_bytes, uint8_t alignment) {
		void* p = _pos;
		char* adjusted = static_cast<char*>(alignForward(p, alignment));
		adjusted[-1] = alignForwardAdjustment(p, alignment); // Store the adjustment 1 byte behind the data.
		_pos = (void*)((char*)_pos + num_bytes + adjusted[-1]);

		// TODO ensure stack cannot overflow into memory it does not own when full. Or resize?
		return adjusted;
	}

	void StackAllocator::revertTo(void* p) {
		_pos = p;
	}

	void StackAllocator::reset() {
		_pos = (void*)((char*)_mem);
	}

	size_t StackAllocator::getCapacity() {
		return _capacity;
	}

	void StackAllocator::dealloc(void* p) {
		// Do nothing, unless block is end of stack. Stack allocator relies solely on reset() or revertTo() for reusing memory.
	}

	void StackAllocator::ref(void* p) {
		// Do nothing. Stack allocator does not reference-count.
	}

	void StackAllocator::deref(void* p) {
		// Do nothing. Stack allocator does not reference-count.
	}
}