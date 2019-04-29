#pragma once
#include "stdafx.h";
#include "MemoryAllocator.h";
#include "Util.h";

// Allocates an aligned chunk of memory in a stack.
class FERROUS_ALLOCATOR_API AlignedStackAllocator {
public:
	typedef void* Marker;

	explicit AlignedStackAllocator(MemoryAllocator* parent_alloc, uint64_t size_bytes, uint64_t alignment);

	~AlignedStackAllocator(void);

	// Allocates the specified number of bytes.
	Marker alloc(uint64_t bytes);

	// Allocates enough bytes to fit the required number of objects at the specified byte-size.
	Marker alloc(uint64_t obj_size_bytes, uint64_t num_obj);

	// Reverts the stack back to the specified marker.
	void releaseToMarker(Marker marker);

	// Reverts the stack back to the beginning.
	void reset();

private:
	MemoryAllocator* m_parent_alloc;
	uint64_t m_size_bytes;
	uint64_t m_adjustment;
	uint64_t m_alignment;
	Marker m_start;
	Marker m_end;
	Marker m_pos;
};