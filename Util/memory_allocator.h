#pragma once
#include "stdafx.h"
#include "util_export.h";

class FERROUS_UTIL_API MemoryAllocator {
public:
	struct FreeBlock {
		uint64_t size;
		FreeBlock* next;

		// Above is followed by block data.
	};

	const static int HEADER_SIZE = sizeof(FreeBlock);

	MemoryAllocator(const uint64_t size_bytes);
	~MemoryAllocator();

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
		release(oldArray);
		oldArray = replacement;

		// TODO this can be sped up a lot if the end of oldArray is followed by a free block capable of fitting the extra capacity. We would reduce fragmentation from allocating and releasing.
	}

	void release(void* p);

	void reset(void);

	void defragment(const uint32_t maxDepth);

	MemoryAllocator::FreeBlock* getFreeList() {
		return m_free;
	}

	uint64_t getAllocated() { return m_allocated; }

	uint64_t getOverhead() { return m_overhead; }

	uint64_t getPageSize() { return m_page_size; }

private:
	uint64_t m_page_size;
	void* m_start;
	FreeBlock* m_free;
	uint64_t m_allocated;
	uint64_t m_overhead;
};