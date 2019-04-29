#include "StackAllocator.h";

StackAllocator::StackAllocator(MemoryAllocator* parent_alloc, uint64_t size_bytes) {
	m_parent_alloc = parent_alloc;
	m_size_bytes = size_bytes;
	m_start = m_parent_alloc->alloc(size_bytes);
	m_pos = m_start;
}

StackAllocator::~StackAllocator(void) {
	if (m_parent_alloc != NULL)
		m_parent_alloc->release(m_start);
}

StackAllocator::Marker StackAllocator::alloc(uint64_t bytes) {
	StackAllocator::Marker p = m_pos;
	m_pos = (void*)((char*)m_pos + bytes);
	return p;
}

void StackAllocator::releaseToMarker(StackAllocator::Marker marker) {
	m_pos = marker;
}

void StackAllocator::reset() {
	m_pos = m_start;
}