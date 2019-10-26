#include "stream_reader.h"

namespace fe {
	StreamReader::StreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize) {
		_stream = stream;
		_allocator = allocator;
		_buffer_size = bufferSize;
		_buffer = nullptr;

		ensureBufferCapacity(bufferSize);
	}

	StreamReader::~StreamReader() {
		_allocator->dealloc(_buffer);
		_stream = nullptr;
	}

	void StreamReader::ensureBufferCapacity(size_t capacity) {
		if (_buffer != nullptr)
			_allocator->dealloc(_buffer);

		_buffer_size = capacity;
		_buffer = _allocator->allocType<char>(capacity);
	}
}