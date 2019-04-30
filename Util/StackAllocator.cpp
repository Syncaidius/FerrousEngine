#include "StackAllocator.h";

StackAllocator::StackAllocator(MemoryAllocator* parent_alloc, uint64_t size_bytes, uint64_t alignment) {
	assert(parent_alloc != nullptr);
	assert(alignment >= 1);
	assert(alignment <= 128);
	assert((alignment & (alignment - 1)) == 0);

	m_alignment = alignment;
	m_parent_alloc = parent_alloc;
	m_size_bytes = size_bytes;

	uint64_t expanded_bytes = size_bytes + alignment;
	m_start = m_parent_alloc->alloc(expanded_bytes);
	uint64_t raw_start = reinterpret_cast<uint64_t>(&m_start);

	//Calc adjustment by masking off the lower bits of address, to determine how "misaligned" it is.
	uint64_t mask = (alignment - 1);
	uint64_t misalignment = (raw_start & mask);
	m_adjustment = alignment - misalignment;
	uint64_t alignedAddress = raw_start + m_adjustment;

	m_pos = reinterpret_cast<void*>(alignedAddress);
}

StackAllocator::~StackAllocator(void) {
	m_parent_alloc->release(m_start);
}

StackAllocator::Marker StackAllocator::alloc(uint64_t bytes) {
	StackAllocator::Marker p = m_pos;
	m_pos = (void*)((char*)m_pos + bytes);
	return m_pos;
}

StackAllocator::Marker StackAllocator::alloc(uint64_t obj_size_bytes, uint64_t num_obj) {
	StackAllocator::Marker p = m_pos;
	m_pos = (void*)((char*)m_pos + (obj_size_bytes * num_obj));
	return m_pos;
}

void StackAllocator::releaseToMarker(StackAllocator::Marker marker) {
	m_pos = marker;
}

void StackAllocator::reset() {
	m_pos = (void*)((char*)m_start + m_adjustment);
}