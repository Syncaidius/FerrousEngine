#include "stream_writer.h"

namespace fe {
	StreamWriter::StreamWriter(Stream* stream) {
		_stream = stream;
	}

	StreamWriter::~StreamWriter() {
		_stream = nullptr;
	}
}