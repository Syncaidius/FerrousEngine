#include "text_stream_writer.h"
namespace fe {
	TextStreamWriter::TextStreamWriter(Stream* stream, FerrousAllocator* allocator) :
		StreamWriter(stream) {
		_allocator = allocator;
		new (&_newLine) UtfString("\n"_fe, stream->getEncoding(), allocator);
	}

	TextStreamWriter::~TextStreamWriter() {}

	void TextStreamWriter::writeLine(const FeString& val) {
		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		UtfString utf = UtfString(val, _stream->getEncoding());
		_stream->writeBytes(utf.getData(), utf.byteLen());
		_stream->writeBytes(_newLine.getData(), _newLine.byteLen());
	}

	void TextStreamWriter::writeLine(const UtfString& val) {
		if (val.byteLen() == 0)
			return;

		if (!_stream->isOpen())
			throw StreamClosedError(_stream);

		UtfEncoding streamEncoding = _stream->getEncoding();
		UtfEncoding valEncoding = val.getEncoding();

		// Do we need to decode then re-encode with the writer's UTF encoding?
		if (valEncoding != streamEncoding) {
			FeString decoded = val.decode();
			UtfString recoded = UtfString(decoded, streamEncoding, _allocator);
			_stream->writeBytes(recoded.getData(), recoded.byteLen());
		}
		else {
			_stream->writeBytes(val.getData(), val.byteLen());
		}

		_stream->writeBytes(_newLine.getData(), _newLine.byteLen());
	}
}