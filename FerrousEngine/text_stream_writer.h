#pragma once
#include "stream_writer.h"

namespace fe {
	class TextStreamWriter : StreamWriter {
	public:
		TextStreamWriter(Stream* stream, FerrousAllocator* allocator);
		virtual ~TextStreamWriter();

		void writeLine(const FeString& str);
		void writeLine(const UtfString& str);
	private:
		FerrousAllocator* _allocator;
		UtfString _newLine;
	};
}