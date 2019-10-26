#include "stream_binary_reader.h"

namespace fe {
	BinaryStreamReader::BinaryStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize) :
		StreamReader(stream, allocator, bufferSize) {

	}

	BinaryStreamReader::~BinaryStreamReader() {

	}

	void BinaryStreamReader::readString(FeString* val) {

	}
}