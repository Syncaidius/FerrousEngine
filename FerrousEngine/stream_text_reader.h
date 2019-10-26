#pragma once
#include "stream_reader.h"

namespace fe {
	class TextStreamReader : StreamReader {
	public:
		TextStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize = 1024);
		TextStreamReader(Stream* stream, size_t bufferSize = 1024);
		virtual ~TextStreamReader();

		void read(FeString* dest, uint32_t numChars);
		void read(UtfString* dest, uint32_t numChars);

		void readLine(FeString* dest);
		void readLine(UtfString* dest);
		void readToEnd(FeString* dest);
		void readToEnd(UtfString* dest);
	};
}