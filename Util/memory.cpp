#include "stdafx.h";
#include "memory.h";

// Static
Memory* Memory::_allocator = new Memory();

Memory::Memory() {
	assert(PAGE_SIZE > PAGE_MIN_OVERHEAD);

	_total_alloc = 0;
	_total_overhead = 0;
	_pages = nullptr;

	_total_alloc_blocks = 0;
	_total_free_blocks = 0;
	_total_pages = 0;
	_pages = newPage();
	_page_to_defrag = _pages;
	reset(false);
}

Memory::~Memory(void) {
	Page* p = _pages;
	while (p != nullptr) {
		Page* next = p->_next;
		free(p);
		p = next;
	}	
}

Memory::Page* Memory::newPage(void) {
	Page* p = reinterpret_cast<Page*>(malloc(PAGE_SIZE));
	_total_pages++;
	_total_overhead += PAGE_MIN_OVERHEAD;
	_total_alloc += PAGE_MIN_OVERHEAD;
	_total_free_blocks++;

	resetPage(p);
	return p;
}

void Memory::resetPage(Page* p) {
	char* data_start = reinterpret_cast<char*>(p) + PAGE_HEADER_SIZE;

	p->_overhead = PAGE_MIN_OVERHEAD;
	p->_allocated = PAGE_MIN_OVERHEAD;
	p->_blocks_allocated = 0;
	p->_blocks_free = 1;
	p->_next = nullptr;

	p->_blocks = reinterpret_cast<Block*>(data_start);
	p->_blocks->_size = PAGE_FREE_SIZE;
	p->_blocks->_ref_count = 0;
	p->_blocks->_next = nullptr;
	p->_blocks->_page = p;
	p->_blocks->_adjustment = 0;
}

void Memory::reset(bool release_pages) {
	Page* p = _pages;

	/* Release all but the first page. */
	if (release_pages) {
		p = _pages->_next;
		Page* next = p->_next;
		while (p != nullptr) {
			next = p->_next;
			free(p);
			p = next;
		}

		_pages->_next = nullptr;
		_total_pages = 1;
	}

	/* Reset all remaining pages */
	p = _pages;
	while (p != nullptr) {
		resetPage(p);
		p = p->_next;
	}

	_page_to_defrag = _pages;
	_total_alloc = _total_pages * PAGE_MIN_OVERHEAD;
	_total_overhead = _total_alloc;
	_total_free_blocks = _total_pages;
	_total_alloc_blocks = 0;
}

void* Memory::allocFast(const size_t size_bytes) {
	assert(size_bytes < PAGE_FREE_SIZE);

	Page* page = _pages;
	while (page != nullptr) {
		// Find a free block which is big enough. If it's too big, downsize it accordingly.
		// This is not as fast as a specialized allocator (e.g. stack).
		Block* block = page->_blocks;
		Block* prev = nullptr;
		void* p = nullptr;

		while (block != nullptr) {
			if (block->_size < size_bytes) {
				prev = block;
				block = block->_next;
				//assert(block != block->next);
				continue;
			}

			block->_adjustment = 0;
			p = (void*)((char*)block + BLOCK_HEADER_SIZE);
			uint64_t remaining = block->_size - size_bytes;

			// Throw leftover memory into a new block if the current block was larger than needed.
			if (remaining > BLOCK_HEADER_SIZE) {
				Block* remainBlock = (Block*)((char*)block + BLOCK_HEADER_SIZE + size_bytes);
				remainBlock->_size = remaining - BLOCK_HEADER_SIZE;
				remainBlock->_next = block->_next;

				if (block == page->_blocks) {
					page->_blocks = remainBlock;
				}
				else {
					if (prev != nullptr)
						prev->_next = remainBlock;
				}
				// Update size of allocated block
				block->_size = size_bytes;

				// We've just allocated a new header and the requested # of bytes.
				page->_allocated += size_bytes + BLOCK_HEADER_SIZE;
				page->_overhead += BLOCK_HEADER_SIZE;

				_total_alloc += size_bytes + BLOCK_HEADER_SIZE;
				_total_overhead += BLOCK_HEADER_SIZE;
			}
			else { 
				// TODO if the remaining size is > 0 (but less-than-or-equal-to HEADER_SIZE) throw it into an physically-adjacent block (if free).

				// Take the whole block, including the leftover/extra bytes.
				page->_allocated += block->_size;
				_total_alloc += block->_size;

				if (block == page->_blocks) {
					page->_blocks = block->_next;
				}
				else {
					if (prev != nullptr)
						prev->_next = block->_next;
				}

				_total_free_blocks--;
				page->_blocks_free--;
			}

			// Update stats
			block->_page = page;
			block->_ref_count = 1;

			page->_blocks_allocated++;

			_total_alloc_blocks++;
			return p;
		}

		/* Out of allocated memory, or lack of adequate-sized blocks? Allocate a new page. */
		if (page->_next == nullptr) {
			page->_next = newPage();
		}

		page = page->_next;
	}

	assert(false); // Should not have reached this point. More memory is allocated if we ran out.
	return nullptr;
}

void* Memory::alloc(const size_t size_bytes) {
	void* p = allocFast(size_bytes);
	memset(p, 0, size_bytes);
	return p;
}

void Memory::ref(void* p) {
	Block* b = getHeader(p);
	b->_ref_count++;
}

void Memory::deref(void* p) {
	Block* b = getHeader(p);
	b->_ref_count--;
	if (b->_ref_count == 0)
		dealloc(p);
}

void Memory::realloc(void*& target, const size_t old_num_bytes, const size_t num_bytes) {
	void* mem = alloc(num_bytes);
	memcpy(mem, target, old_num_bytes);
	dealloc(target);
	target = mem;
}

void Memory::copy(void* dest, const void* src, const size_t num_bytes) {
	// TODO some validation?
	memcpy(dest, src, num_bytes);
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
	size_t _adjustment = alignment - misalignment;
	uintptr_t alignedAddr = raw_start + alignment;

	Block * block = reinterpret_cast<Block*>((char*)p - BLOCK_HEADER_SIZE);
	block->_adjustment = alignment + start_offset;
	return reinterpret_cast<void*>(alignedAddr);
}

void Memory::dealloc(void* p) {
	assert(p != nullptr);
	Block* blk = getHeader(p);
	Page* page = blk->_page;
	assert(blk != page->_blocks);

	page->_allocated -= blk->_size;
	_total_alloc -= blk->_size;

	if (!tryMerge(page, blk, page->_blocks)) {
		blk->_next = page->_blocks;
		page->_blocks_free++;
		_total_free_blocks++;
	}

	page->_blocks = blk; // Free list starts at released block, regardless of if it merged or not.
	page->_blocks_allocated--;
	_total_alloc_blocks--;
}

void Memory::defragment(int iterations) {
	size_t num_done = 0;

	for(int i = 0; i < iterations; i++)
	{
		mergeSort(&_page_to_defrag->_blocks);
		Block* prev = _page_to_defrag->_blocks;
		Block* cur = _page_to_defrag->_blocks->_next;

		while (cur != nullptr) {
			uintptr_t ending = reinterpret_cast<uintptr_t>(prev) + BLOCK_HEADER_SIZE + prev->_size;
			uintptr_t start = reinterpret_cast<uintptr_t>(cur);

			if (tryMerge(_page_to_defrag, prev, cur)) {
				cur = prev;
			}
			else {
				prev = cur; // cur was not absorbed, so that becomes the previous block.
			}

			cur = cur->_next;
		}

		/* Ensure we don't end up cycling over the same few pages,
		if iteration count is higher than total page count. */
		num_done++;
		if (num_done < _total_pages) {
			if (_page_to_defrag->_next != nullptr) {
				_page_to_defrag = _page_to_defrag->_next;
			}
			else {
				_page_to_defrag = _pages;
			}
		}
		else {
			break;
		}
	}
}

bool Memory::tryMerge(Page* page, Block* prev, Block* cur) {
	uintptr_t ending = reinterpret_cast<uintptr_t>(prev) + BLOCK_HEADER_SIZE + prev->_size;
	uintptr_t start = reinterpret_cast<uintptr_t>(cur);

	if (start == ending) {
		page->_overhead -= BLOCK_HEADER_SIZE;
		page->_allocated -= BLOCK_HEADER_SIZE;

		_total_overhead -= BLOCK_HEADER_SIZE;
		_total_alloc -= BLOCK_HEADER_SIZE;

		prev->_size += cur->_size + BLOCK_HEADER_SIZE; // Include header of 'other' too
		prev->_next = cur->_next; // Detach cur
		return true;
	}

	return false;
}

void Memory::mergeSort(Block** headRef) {
	Block* head = *headRef;
	Block* a;
	Block* b;

	/* Base case -- length 0 or 1 */
	if ((head == nullptr) || (head->_next == nullptr))
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
		result->_next = sortedMerge(a->_next, b);
	}
	else {
		result = b;
		result->_next = sortedMerge(a, b->_next);
	}

	return result;
}

void Memory::frontBackSplit(Block* source, Block** frontRef, Block** backRef) {
	Block* fast;
	Block* slow;
	slow = source;
	fast = source->_next;

	/*Advance fast by 2 nodes, slow by 1.*/
	while (fast != nullptr) {
		fast = fast->_next;
		if (fast != nullptr) {
			slow = slow->_next;
			fast = fast->_next;
		}
	}

	/* slow is before the midpoint in the list, split in two */
	*frontRef = source;
	*backRef = slow->_next;
	slow->_next = nullptr;
}

Memory::Block* Memory::getHeader(void* p) {
	// Header is located behind the pointer's address.
	size_t adjustment_loc = BLOCK_HEADER_SIZE - offsetof(struct Block, _adjustment);
	uint8_t* _adjustment = reinterpret_cast<uint8_t*>((char*)p - adjustment_loc);
	return reinterpret_cast<Block*>((char*)p - *_adjustment - BLOCK_HEADER_SIZE);
}