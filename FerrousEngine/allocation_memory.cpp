#include "allocation.h"
#include <iostream>
using namespace std;

Memory* Memory::_allocator = new Memory();

Memory::Memory() {
	assert(PAGE_SIZE > (BLOCK_HEADER_SIZE + PAGE_HEADER_SIZE)); // Page size too small.

	_pages = nullptr;
	_page_count = 0;
	_blocks = newPage();
	_page_to_defrag = _pages;
}

Memory::~Memory() {
	Page* p = _pages;
	while (p != nullptr) {
		Page* next = p->_next;
		free(p);
		p = next;
	}
}

Memory::Block* Memory::makePageBlock(Page* p) {
	char* page_data = reinterpret_cast<char*>(p) + PAGE_HEADER_SIZE;
	Block* b = reinterpret_cast<Block*>(page_data);
	b->_size = PAGE_SIZE - PAGE_HEADER_SIZE - BLOCK_HEADER_SIZE;
	b->_next = nullptr;
	return b;
}

Memory::Block* Memory::newPage(void) {
	void* mem = malloc(PAGE_SIZE);

	if (mem != nullptr) {
		Page* p = static_cast<Page*>(mem);
		p->_next = _pages;
		_pages = p;
		_page_count++;
		return makePageBlock(p);
	}
	else {
		throw exception("Out of memory.");
	}
}

void Memory::reset(void) {
	Page* p = _pages;
	_page_to_defrag = _pages;
	_blocks = nullptr;

	while (p != nullptr) {
		Block* b = makePageBlock(p);
		if (_blocks == nullptr) {
			_blocks = b;
		}
		else {
			b->_next = _blocks;
			_blocks = b;
		}

		p = p->_next;
	}
}

void* Memory::alloc(size_t num_bytes, uint8_t alignment) {
	assert(alignment > 0);
	num_bytes += alignment;
	assert(num_bytes < PAGE_FREE_SIZE); // Page size not large enough.

	Block* b = _blocks;
	Block* prev = nullptr;

	while (b != nullptr) {
		if (b->_size < num_bytes) {
			prev = b;
			if (b->_next == nullptr) // Get new page?
				b->_next = newPage();

			b = b->_next;
			continue;
		}

		Block* result = b;
		size_t new_block_bytes = num_bytes + BLOCK_HEADER_SIZE;
		if (b->_size > new_block_bytes) { // Split off num_bytes into a new block
			b->_size -= new_block_bytes;
			result = reinterpret_cast<Block*>(reinterpret_cast<char*>(b) + BLOCK_HEADER_SIZE + b->_size);
			result->_size = num_bytes;
		}
		else { // Take the whole block
			if (b == _blocks) {
				_blocks = b->_next;
				//assert(_blocks != nullptr);
				if (_blocks == nullptr)
					_blocks = newPage();
			}
			else if (prev != nullptr) {
				prev->_next = b->_next;
			}
		}

		// Update stats
		result->_info._ref_count = 1;

		// Set adjustment
		char* p = reinterpret_cast<char*>(result) + BLOCK_HEADER_SIZE;
		char* adjusted = static_cast<char*>(alignForward(p, alignment));
		adjusted[-1] = alignForwardAdjustment(p, alignment); // Store the adjustment 1 byte behind the data.
		return static_cast<void*>(adjusted);
	}

	assert(false); // Should not have reached this point. More memory is allocated if we ran out.
	return nullptr;
}

void Memory::dealloc(void* p) {
	char* temp = static_cast<char*>(p);
	Block* b = reinterpret_cast<Block*>(temp - temp[-1] - BLOCK_HEADER_SIZE);
	b->_next = _blocks;
	_blocks = b;
}

void Memory::realloc(void*& target, const size_t num_bytes, uint8_t alignment) {
	char* temp = static_cast<char*>(target);
	uint8_t adjustment = temp[-1];
	Block* b = reinterpret_cast<Block*>(temp - adjustment - BLOCK_HEADER_SIZE);

	void* mem = alloc(num_bytes, alignment);
	memcpy(mem, target, b->_size - adjustment);
	deref(target);
	target = mem;
}

void Memory::ref(void* p) {
	char* temp = static_cast<char*>(p);
	Block* b = reinterpret_cast<Block*>(temp - temp[-1] - BLOCK_HEADER_SIZE);
	b->_info._ref_count++;
}

void Memory::deref(void* p) {
	char* temp = static_cast<char*>(p);
	Block* b = reinterpret_cast<Block*>(temp - temp[-1] - BLOCK_HEADER_SIZE);
	b->_info._ref_count--;
	if (b->_info._ref_count == 0)
		dealloc(p);
}

size_t Memory::getCapacity() {
	return Memory::PAGE_SIZE* _page_count;
}

void Memory::copy(void* dest, const void* src, const size_t num_bytes) {
	// TODO some validation?
	memcpy(dest, src, num_bytes);
}

void Memory::defragment(size_t max_pages) {
	assert(max_pages > 0);
	Page* p = _page_to_defrag;
	for (size_t i = 0; i < max_pages; i++) {
		mergeSort(&_blocks);

		Block* b = _blocks->_next;
		Block* prev = _blocks;

		while (b != nullptr) {
			if (canMerge(prev, b)) {
				prev->_size += b->_size + BLOCK_HEADER_SIZE;
				prev->_next = b->_next; // link the 'prev' block to the block after 'cur'
			}
			else {
				prev = b;
			}

			b = b->_next;
		}

		// Next page, or reset back to the first if we're done.
		p = _page_to_defrag->_next;
		if (p == nullptr) {
			_page_to_defrag = _pages;
			break;
		}
	}
}

bool Memory::canMerge(Block * prev, Block * cur) {
	uintptr_t start = reinterpret_cast<uintptr_t>(cur);
	uintptr_t ending = reinterpret_cast<uintptr_t>(prev) + BLOCK_HEADER_SIZE + prev->_size;
	return start == ending;
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

void Memory::outputDebug() {
	cout << "Memory allocator analysis" << endl;
	cout << "=========================" << endl;
	size_t capacity = _page_count * PAGE_SIZE;	
	Block* b = _blocks;
	uint32_t total_blocks = 0;
	size_t total_free_bytes = 0;

	while (b != nullptr) {
		//cout << "Block " << reinterpret_cast<uintptr_t>(b) << " -- " << b->_size << " bytes" << endl;
		total_blocks++;
		total_free_bytes += b->_size;
		b = b->_next;
	}
	
	cout << "Allocated system memory: " << capacity << " bytes" << endl;
	cout << "Allocated free: " << total_free_bytes << " bytes" << endl;
	cout << "Total pages: " << _page_count << endl;
	cout << "Total blocks: " << total_blocks << endl;
}