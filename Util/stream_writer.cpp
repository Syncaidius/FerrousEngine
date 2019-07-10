#include "stream_writer.h"
StreamWriter::StreamWriter(Stream* stream) {
	_stream = stream;
}

StreamWriter::~StreamWriter() {
	_stream = nullptr;
}