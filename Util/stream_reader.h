#pragma once
#include "stream.h"

class FERROUS_UTIL_API StreamReader {
public:
	StreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize = 1024);
	virtual ~StreamReader();

	inline Stream* getStream() { return _stream; }
	inline size_t getBufferSize() { return _buffer_size; }
protected:
	Stream* _stream;
	char* _buffer;
	size_t _buffer_size;
	FerrousAllocator* _allocator;
};
