#include "stdafx.h";
#include "memory_allocator.h";

MemoryAllocator::MemoryAllocator(const uint64_t size_bytes) {
	assert(size_bytes > MemoryAllocator::HEADER_SIZE);

	m_page_size = size_bytes;
	m_start = malloc(size_bytes);
	reset();
}

MemoryAllocator::~MemoryAllocator(void) {
	free(m_start);
}

void* MemoryAllocator::alloc(const uint64_t size_bytes) {
	// Find a free block which is big enough. If it's too big, downsize it accordingly.
	// This is not as fast as a specialized allocator (e.g. stack).
	FreeBlock* block = m_free;
	void* p = nullptr;

	while (block != nullptr) {
		if (block->size < size_bytes) {
			block = block->next;
			continue;
		}

		p = (void*)((char*)block + HEADER_SIZE);
		uint64_t remaining = block->size - size_bytes;

		// Throw leftover memory into a new block if the current block was larger than needed.
		if (remaining > HEADER_SIZE) {
			// Set new block header for remaining bytes
			m_free = (FreeBlock*)((char*)block + HEADER_SIZE + size_bytes);
			m_free->size = remaining - HEADER_SIZE;
			m_free->next = block->next;

			// Update size of allocated block
			block->size = size_bytes;

			// We've just allocated a new header and the requested bytes.
			m_allocated += size_bytes + HEADER_SIZE;
			m_overhead += HEADER_SIZE;
		}
		else { // TODO if the remaining size is > 0 but less-than-or-equal-to HEADER_SIZE, can we throw it into the block after it (if exists)?
			m_allocated += m_free->size;
			m_free = block->next;
		}

		break;
	}

	assert(p != nullptr);
	return p;
}

void MemoryAllocator::release(void* p) {
	// Header is located behind the pointer's address.
	FreeBlock* block_to_free = reinterpret_cast<FreeBlock*>((char*)p - HEADER_SIZE);
	m_allocated -= block_to_free->size;
	block_to_free->next = m_free;
	m_free = block_to_free;
}

void MemoryAllocator::reset(void) {
	// We now have just one header, so subtract one header worth of bytes from the allocator.
	m_free = reinterpret_cast<FreeBlock*>(m_start);
	m_free->size = m_page_size - HEADER_SIZE;
	m_free->next = nullptr;
	m_overhead = HEADER_SIZE;
	m_allocated = HEADER_SIZE;
}

void MemoryAllocator::defragment(const uint32_t maxDepth) {
	FreeBlock* block = m_free;
	FreeBlock* other = nullptr;
	FreeBlock* prev_other = nullptr;

	while(block != nullptr) {
		other = m_free;
		prev_other = nullptr;

		while (other != nullptr) {
			if (other != block) {
				uint64_t ending = reinterpret_cast<uint64_t>(block) + HEADER_SIZE + block->size;
				uint64_t start = reinterpret_cast<uint64_t>(other);

				if (start == ending) {
					m_overhead -= HEADER_SIZE;
					m_allocated -= HEADER_SIZE;

					if (block->next == other) {
						block->next = other->next;
					}else{
						// If the previous 'other' was not 'block', we've broken the link of a block between 'block' and 'other. Fix it.
						if(prev_other != nullptr)
							prev_other->next = other->next;
					}

					block->size += other->size + HEADER_SIZE; // Include header of 'other' too
					if (other == m_free) {
						m_free = block;
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
