#pragma once
#include "stdafx.h";
#include "MemoryAllocator.h";
#include "Util.h";

// Allocates a chunk of memory in a stack.
class FERROUS_ALLOCATOR_API StackAllocator {
public:
	typedef void* Marker;

	explicit StackAllocator(MemoryAllocator* parent_alloc, uint64_t size_bytes);

	~StackAllocator(void);

	// Allocates the specified number of bytes.
	Marker alloc(uint64_t bytes);

	// Reverts the stack back to the specified marker.
	void releaseToMarker(Marker marker);

	// Reverts the stack back to the beginning.
	void reset();

private:
	uint64_t			m_size_bytes;
	Marker				m_start;
	Marker				m_pos;
	MemoryAllocator* m_parent_alloc;
};