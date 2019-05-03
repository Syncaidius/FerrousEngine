#include "stdafx.h";
#include "memory_allocator.h";

// Static
MemoryAllocator* MemoryAllocator::_allocator = new MemoryAllocator();

MemoryAllocator::MemoryAllocator() {
	assert(PAGE_SIZE > HEADER_SIZE);

	_start = malloc(PAGE_SIZE);
	reset();
}

MemoryAllocator::~MemoryAllocator(void) {
	free(_start);
}

void* MemoryAllocator::alloc(const uint64_t size_bytes) {
	// Find a free block which is big enough. If it's too big, downsize it accordingly.
	// This is not as fast as a specialized allocator (e.g. stack).
	FreeBlock* block = _free;
	FreeBlock* prev = nullptr;
	void* p = nullptr;

	while (block != nullptr) {
		if (block->size < size_bytes) {
			prev = block;
			block = block->next;
			continue;
		}

		p = (void*)((char*)block + HEADER_SIZE);
		uint64_t remaining = block->size - size_bytes;

		// Throw leftover memory into a new block if the current block was larger than needed.
		if (remaining > HEADER_SIZE) {
			FreeBlock* remainBlock = (FreeBlock*)((char*)block + HEADER_SIZE + size_bytes);
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

			if(block == _free)
				_free = block->next;
		}

		break;
	}

	assert(p != nullptr);
	memset(p, 0, size_bytes);
	return p;
}

void MemoryAllocator::dealloc(void* p) {
	// Header is located behind the pointer's address.
	FreeBlock* block_to_free = reinterpret_cast<FreeBlock*>((char*)p - HEADER_SIZE);
	_allocated -= block_to_free->size;
	block_to_free->next = _free;
	_free = block_to_free;
}

void MemoryAllocator::reset(void) {
	// We now have just one header, so subtract one header worth of bytes from the allocator.
	_free = reinterpret_cast<FreeBlock*>(_start);
	_free->size = PAGE_SIZE - HEADER_SIZE;
	_free->next = nullptr;
	_overhead = HEADER_SIZE;
	_allocated = HEADER_SIZE;
}

void MemoryAllocator::defragment(const uint32_t maxDepth) {
	FreeBlock* block = _free;
	FreeBlock* other = nullptr;
	FreeBlock* prev_other = nullptr;

	while(block != nullptr) {
		other = _free;
		prev_other = nullptr;

		while (other != nullptr) {
			if (other != block) {
				uint64_t ending = reinterpret_cast<uint64_t>(block) + HEADER_SIZE + block->size;
				uint64_t start = reinterpret_cast<uint64_t>(other);

				if (start == ending) {
					_overhead -= HEADER_SIZE;
					_allocated -= HEADER_SIZE;

					if (block->next == other) {
						block->next = other->next;
					}else{
						// If the previous 'other' was not 'block', we've broken the link of a block between 'block' and 'other. Fix it.
						if(prev_other != nullptr)
							prev_other->next = other->next;
					}

					block->size += other->size + HEADER_SIZE; // Include header of 'other' too
					if (other == _free) {
						_free = block;
					}
				}
				else {
					prev_other = other;
				}
			}

			other = other->next;
		}

		block = block->next;
	}
}

/* TODO:
	-- Will we ever need more than 2GB in a single allocated block?
		- Overhead could be reduced by 25% by changing FreeBlock->size to uint32_t instead of uint64_t. 12 bytes header size instead of 16.
	-- Ability to directly allocate memory stacks by moving StackAllocator functionlity into a nested type of MemoryAllocator.
		- Currently they have to be instantiated/initialized by passing a MemoryAllocator to their constructor.
*/
