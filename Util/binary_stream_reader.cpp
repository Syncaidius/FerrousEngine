#include "binary_stream_reader.h"

BinaryStreamReader::BinaryStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize) : 
	StreamReader(stream, allocator, bufferSize) {

}

BinaryStreamReader::~BinaryStreamReader() {

}

void BinaryStreamReader::readString(FeString* val) {

}