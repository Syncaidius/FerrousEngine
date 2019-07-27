#include "stream_reader.h"

StreamReader::StreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize) {
	_stream = stream;
	_buffer_size = bufferSize;
	_allocator = allocator;
	_buffer = _allocator->allocType<char>(_buffer_size);
}

StreamReader::~StreamReader() {
	_allocator->dealloc(_buffer);
	_stream = nullptr;
}