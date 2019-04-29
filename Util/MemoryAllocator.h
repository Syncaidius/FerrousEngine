#pragma once
#include "stdafx.h"
#include "Util.h";

class FERROUS_ALLOCATOR_API MemoryAllocator {
public:
	struct FreeBlock {
		uint64_t size;
		FreeBlock* next;

		// Above is followed by block data.
	};

	const static int HEADER_SIZE = sizeof(FreeBlock);
	typedef void* Marker;

	MemoryAllocator(uint64_t size_bytes);

	~MemoryAllocator(void);

	Marker alloc(uint64_t size_bytes);

	void release(Marker marker);

	void reset(void);

	FreeBlock* getFreeList();

	uint64_t getAllocated();

	uint64_t getOverhead();

	uint64_t getSize();

	void defragment(uint32_t maxDepth);
private:

	uint64_t m_size_bytes;
	Marker m_start;
	FreeBlock* m_free;
	uint64_t m_allocated;
	uint64_t m_overhead;
};