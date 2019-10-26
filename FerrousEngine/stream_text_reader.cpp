#include "stream_text_reader.h"

namespace fe {
	TextStreamReader::TextStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize) :
		StreamReader(stream, allocator, bufferSize) {
	}

	TextStreamReader::TextStreamReader(Stream* stream, size_t bufferSize) :
		TextStreamReader(stream, Memory::get(), bufferSize) { }

	TextStreamReader::~TextStreamReader() {}

	void TextStreamReader::read(FeString* dest, uint32_t numChars) {
		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		if (!_stream->canRead())
			throw StreamReadAccessError(_stream);

		// Read (4 * count) bytes to cover worst-case scenario of 4 bytes per character.
		// UTF-8, 16 and 32 all use a max of 4 bytes per character.
		size_t bytesToRead = min(_stream->getReadBytesFromEnd(), (size_t)numChars * 4U);
		char* c = _buffer;

		ensureBufferCapacity(bytesToRead);
		_stream->readBytes(_buffer, bytesToRead);

		*dest = UtfString::decode(_buffer, numChars, _stream->getEncoding(), _allocator);

		// Move the stream backwards by the number of bytes that were left over after decoding.
		int32_t numBytes = (int32_t)(c - (dest->len() * sizeof(wchar_t)));
		_stream->seekRead(StreamSeek::Current, -numBytes);
	}

	void TextStreamReader::readLine(FeString* dest) {
		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		if (!_stream->canRead())
			throw StreamReadAccessError(_stream);

		char* c = _buffer;
		size_t numBytes = 0;
		_stream->readBytes(c, 1);

		size_t end = _stream->getSize();

		while (*c != '\n') {
			if (_stream->getReadPos() > end)
				throw EndOfStreamError(_stream);

			c++;
			numBytes++;
			_stream->readBytes(c, 1);
		}

		*dest = UtfString::decode(_buffer, _stream->getEncoding(), numBytes, _allocator);
	}

	void TextStreamReader::readLine(UtfString* dest) {
		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		if (!_stream->canRead())
			throw StreamReadAccessError(_stream);

		char* c = _buffer;
		size_t numBytes = 0;
		_stream->readBytes(c, 1);

		while (*c != '\n' && *c != EOF) {
			c++;
			numBytes++;
			assert(numBytes < _buffer_size);
			_stream->readBytes(c, 1);
		}

		if (*c == EOF)
			throw EndOfStreamError(_stream);

		char* mem = _allocator->allocType<char>(numBytes);
		Memory::copy(mem, _buffer, numBytes);
		new (dest) UtfString(mem, numBytes, _stream->getEncoding(), _allocator);
	}

	void TextStreamReader::readToEnd(FeString * dest) {
		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		if (!_stream->canRead())
			throw StreamReadAccessError(_stream);

		size_t curPos = _stream->getReadPos();
		size_t bytesToRead = _stream->getSize() - curPos;
		_stream->readBytes(_buffer, bytesToRead);
		*dest = UtfString::decode(_buffer, _stream->getEncoding(), bytesToRead, _allocator);
	}

	void TextStreamReader::readToEnd(UtfString * dest) {
		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		if (!_stream->canRead())
			throw StreamReadAccessError(_stream);

		size_t curPos = _stream->getReadPos();
		size_t bytesToRead = _stream->getSize() - curPos;
		ensureBufferCapacity(bytesToRead);

		_stream->readBytes(_buffer, bytesToRead);
		new (dest) UtfString(_buffer, bytesToRead, _stream->getEncoding(), _allocator);
	}
}