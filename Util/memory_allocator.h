#pragma once
#include "stdafx.h"
#include "util.h";

class FERROUS_UTIL_API MemoryAllocator {
public:
	struct FreeBlock {
		uint64_t size;
		FreeBlock* next;

		// Above is followed by block data.
	};

	const static int HEADER_SIZE = sizeof(FreeBlock);
	const static int PAGE_SIZE = 8912;

	void* alloc(const uint64_t size_bytes);

	/*Allocates a new block of memory capable of fitting num_elements of type T. */
	template<typename T> T* allocArray(const uint64_t num_elements) {
 		void* mem = alloc(sizeof(T) * num_elements);
		memset(mem, 0, sizeof(T));
		return reinterpret_cast<T*>(mem);
	}

	/*Allocates a new block of memory capable of fitting num_elements of type T, then copies the old one to it. Once complete, the memory of the old array is released for reuse.
	Updates the oldArray pointer to point to the newly-resized array.*/
	template<typename T> void resizeArray(T*& oldArray, const uint64_t old_num_elements, const uint64_t num_elements) {
		void* mem = alloc(sizeof(T) * num_elements);

		T* replacement = reinterpret_cast<T*>(mem);
		memset(replacement, 0, sizeof(T) * num_elements);
		memcpy(replacement, oldArray, sizeof(T) * old_num_elements);
		dealloc(oldArray);
		oldArray = replacement;

		// TODO this can be sped up a lot if the end of oldArray is followed by a free block capable of fitting the extra capacity. We would reduce fragmentation from allocating and releasing.
	}

	void dealloc(void* p);

	void reset(void);

	void defragment(void);

	MemoryAllocator::FreeBlock* getFreeList() {
		return _free;
	}

	uint64_t getAllocated() { return _allocated; }

	uint64_t getOverhead() { return _overhead; }

	static MemoryAllocator* get() { return _allocator; }

private:
	static MemoryAllocator* _allocator;
	MemoryAllocator();
	~MemoryAllocator();

	void sortFree(void);

	void* _start;
	FreeBlock* _free;
	uint64_t _allocated;
	uint64_t _overhead;
};