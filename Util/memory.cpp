#include "stdafx.h";
#include "memory.h";

// Static
Memory* Memory::_allocator = new Memory();

Memory::Memory() {
	assert(PAGE_SIZE > HEADER_SIZE);

	_start = malloc(PAGE_SIZE);
	reset();
}

Memory::~Memory(void) {
	free(_start);
}

void* Memory::allocFast(const size_t size_bytes) {
	// Find a free block which is big enough. If it's too big, downsize it accordingly.
	// This is not as fast as a specialized allocator (e.g. stack).
	Block* block = _free;
	Block* prev = nullptr;
	void* p = nullptr;

	while (block != nullptr) {
		if (block->size < size_bytes) {
			prev = block;
			block = block->next;
			assert(block != block->next);
			continue;
		}

		block->adjustment = 0;
		p = (void*)((char*)block + HEADER_SIZE);
		uint64_t remaining = block->size - size_bytes;

		// Throw leftover memory into a new block if the current block was larger than needed.
		if (remaining > HEADER_SIZE) {
			Block* remainBlock = (Block*)((char*)block + HEADER_SIZE + size_bytes);
			remainBlock->size = remaining - HEADER_SIZE;
			remainBlock->next = block->next;

			if (block == _free) {
				_free = remainBlock;
			}
			else {
				if (prev != nullptr)
					prev->next = remainBlock;
			}
			// Update size of allocated block
			block->size = size_bytes;

			// We've just allocated a new header and the requested # of bytes.
			_allocated += size_bytes + HEADER_SIZE;
			_overhead += HEADER_SIZE;
		}
		else { // TODO if the remaining size is > 0 but less-than-or-equal-to HEADER_SIZE, can we throw it into the block after it (if exists)
			_allocated += block->size;

			if (block == _free) {
				_free = block->next;
			}
			else {
				if (prev != nullptr)
					prev->next = block->next;
			}
		}

		break;
	}

	assert(p != nullptr);
	return p;
}

void* Memory::alloc(const size_t size_bytes) {
	void* p = allocFast(size_bytes);
	memset(p, 0, size_bytes);
	return p;
}

void Memory::realloc(void*& target, const size_t old_num_bytes, const size_t num_bytes) {
	void* mem = alloc(num_bytes);
	memcpy(mem, target, old_num_bytes);
	dealloc(target);
	target = mem;

	// TODO this can be sped up a lot if the end of target is followed by a free block capable of fitting the extra capacity. We would reduce fragmentation from allocating and releasing.
	//	-- This would also require tracking all memory blocks (free and allocated) in a second linked list in order to immediately detect whether an adjacent block is free or not.
	//		-- ^ would slow down defragmentation, maybe.
	//		-- Perhaps wrap free-list entries in node containers, then have Blocks in a linked list of their own.
}

void* Memory::allocAligned(const size_t size_bytes, const uint8_t alignment) {
	assert(alignment >= 1);
	assert(alignment <= 128);
	assert((alignment & (alignment - 1)) == 0);;

	size_t expanded_bytes = size_bytes + alignment;
	void* p = alloc(expanded_bytes);
	return align(p, alignment, 0);
}

/* Allocates a memory stack with with the specified number of bytes, in a single contiguous block.*/
Memory::Stack* Memory::allocStack(const size_t size_bytes, const uint8_t alignment = 1) {
	size_t stack_header = sizeof(Stack);
	size_t total_bytes = size_bytes + stack_header + alignment;
	void* p = alloc(total_bytes);
	void* aligned = align(p, alignment, stack_header);
	return new (p) Stack(this, aligned);
}

void* Memory::align(void* p, uint8_t alignment, size_t start_offset) {
	uintptr_t raw_start = reinterpret_cast<uintptr_t>(p) + start_offset;

	/* Calc adjustment by masking off the lower bits of address, to determine how "misaligned" it is.*/
	size_t mask = (alignment - 1);
	size_t misalignment = (raw_start & mask);
	size_t adjustment = alignment - misalignment;
	uintptr_t alignedAddr = raw_start + alignment;

	Block * block = reinterpret_cast<Block*>((char*)p - HEADER_SIZE);
	block->adjustment = alignment + start_offset;
	return reinterpret_cast<void*>(alignedAddr);
}

void Memory::dealloc(void* p) {
	Block* block_to_free = getHeader(p);

	assert(block_to_free != _free);
	_allocated -= block_to_free->size;
	block_to_free->next = _free;
	_free = block_to_free;
}

void Memory::reset(void) {
	// We now have just one header, so subtract one header worth of bytes from the allocator.
	_free = reinterpret_cast<Block*>(_start);
	_free->size = PAGE_SIZE - HEADER_SIZE;
	_free->next = nullptr;
	_overhead = HEADER_SIZE;
	_allocated = HEADER_SIZE;
}

void Memory::defragment(void) {
	mergeSort(&_free);
	Block* prev = _free;
	Block* cur = _free->next;

	while (cur != nullptr) {
		uintptr_t ending = reinterpret_cast<uintptr_t>(prev) + HEADER_SIZE + prev->size;
		uintptr_t start = reinterpret_cast<uintptr_t>(cur);

		if (start == ending) {
			_overhead -= HEADER_SIZE;
			_allocated -= HEADER_SIZE;
			prev->size += cur->size + HEADER_SIZE; // Include header of 'other' too
			prev->next = cur->next; // Detach cur
			cur = prev;
		}
		else {
			// cur was not absorbed, so that becomes the previous block.
			prev = cur;
		}

		cur = cur->next;
	}
}

void Memory::mergeSort(Block** headRef) {
	Block* head = *headRef;
	Block* a;
	Block* b;

	/* Base case -- length 0 or 1 */
	if ((head == nullptr) || (head->next == nullptr))
		return;

	frontBackSplit(head, &a, &b);

	//Recursive sort on sublists
	mergeSort(&a);
	mergeSort(&b);

	*headRef = sortedMerge(a, b);
}

Memory::Block* Memory::sortedMerge(Block* a, Block* b) {
	Block* result = nullptr;

	/* base cases*/
	if (a == nullptr)
		return b;
	else if (b == nullptr)
		return a;

	/* Pick either a or b, recursive*/
	if (reinterpret_cast<uintptr_t>(a) <= reinterpret_cast<uintptr_t>(b)) {
		result = a;
		result->next = sortedMerge(a->next, b);
	}
	else {
		result = b;
		result->next = sortedMerge(a, b->next);
	}

	return result;
}

void Memory::frontBackSplit(Block* source, Block** frontRef, Block** backRef) {
	Block* fast;
	Block* slow;
	slow = source;
	fast = source->next;

	/*Advance fast by 2 nodes, slow by 1.*/
	while (fast != nullptr) {
		fast = fast->next;
		if (fast != nullptr) {
			slow = slow->next;
			fast = fast->next;
		}
	}

	/* slow is before the midpoint in the list, split in two */
	*frontRef = source;
	*backRef = slow->next;
	slow->next = nullptr;
}
