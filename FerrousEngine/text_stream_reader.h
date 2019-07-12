#pragma once
#include "stream_reader.h"

class TextStreamReader : StreamReader {
public:
	TextStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize = 1024);
	TextStreamReader(Stream* stream, size_t bufferSize = 1024);
	virtual ~TextStreamReader();

	void readLine(FeString* dest);
	void readLine(UtfString* dest);
	void readToEnd(FeString* dest);
	void readToEnd(UtfString* dest);
};