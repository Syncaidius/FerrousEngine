#include "text_stream_reader.h"

TextStreamReader::TextStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize) :
	StreamReader(stream, allocator, bufferSize) {
}

TextStreamReader::TextStreamReader(Stream* stream, size_t bufferSize) :
	TextStreamReader(stream, Memory::get(), bufferSize) { }

TextStreamReader::~TextStreamReader() {}

void TextStreamReader::readLine(FeString* dest) {
	if (!_stream->isOpen())
		throw StreamClosedError(_stream);

	if (!_stream->canRead())
		throw StreamReadAccessError(_stream);

	char* c = _buffer;
	size_t num_bytes = 0;
	_stream->readBytes(c, 1);
	int temp = 0;

	size_t end = _stream->getSize();

	while (*c != '\n') {
		if (_stream->getReadPos() > end)
			throw EndOfStreamError(_stream);

		c++;
		num_bytes++;
		_stream->readBytes(c, 1);
	}

	*dest = UtfString::decode(_buffer, _stream->getEncoding(), num_bytes, _allocator);
}

void TextStreamReader::readLine(UtfString* dest) {
	if (!_stream->isOpen())
		throw StreamClosedError(_stream);

	if (!_stream->canRead())
		throw StreamReadAccessError(_stream);

	char* c = _buffer;
	size_t num_bytes = 0;
	_stream->readBytes(c, 1);

	while (*c != '\n' && *c != EOF) {
		c++;
		num_bytes++;
		assert(num_bytes < _buffer_size);
		_stream->readBytes(c, 1);
	}

	if (*c == EOF)
		throw EndOfStreamError(_stream);

	char* mem = static_cast<char*>(_allocator->alloc(num_bytes));
	Memory::copy(mem, _buffer, num_bytes);
	new (dest) UtfString(mem, num_bytes, _stream->getEncoding(), _allocator);
}

void TextStreamReader::readToEnd(FeString* dest) {
	if (!_stream->isOpen())
		throw StreamClosedError(_stream);

	if (!_stream->canRead())
		throw StreamReadAccessError(_stream);

	size_t curPos = _stream->getReadPos();
	size_t bytes_to_read = _stream->getSize() - curPos;
	_stream->readBytes(_buffer, bytes_to_read);
	*dest = UtfString::decode(_buffer, _stream->getEncoding(), bytes_to_read, _allocator);
}

void TextStreamReader::readToEnd(UtfString* dest) {
	if (!_stream->isOpen())
		throw StreamClosedError(_stream);

	if (!_stream->canRead())
		throw StreamReadAccessError(_stream);

	size_t curPos = _stream->getReadPos();
	size_t bytes_to_read = _stream->getSize() - curPos;
	char* mem = static_cast<char*>(_allocator->alloc(bytes_to_read));
	_stream->readBytes(mem, bytes_to_read);
	new (dest) UtfString(mem, bytes_to_read, _stream->getEncoding(), _allocator);
}