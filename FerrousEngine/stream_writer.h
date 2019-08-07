#pragma once
#include "stream.h";

namespace fe {
	class StreamWriter {
	public:

		StreamWriter(Stream* stream);
		virtual ~StreamWriter();

		inline Stream* getStream() { return _stream; }

	protected:
		Stream* _stream;
	};
}