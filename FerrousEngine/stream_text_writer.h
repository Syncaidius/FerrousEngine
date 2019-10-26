#pragma once
#include "stream_writer.h"

namespace fe {
	class TextStreamWriter : StreamWriter {
	public:
		TextStreamWriter();

		TextStreamWriter(Stream* stream, FerrousAllocator* allocator);
		virtual ~TextStreamWriter();

		void write(const FeString& str);
		void write(const UtfString& str);

		void writeLine(const FeString& str);
		void writeLine(const UtfString& str);
	private:
		FerrousAllocator* _allocator;
		UtfString _newLine;
	};
}