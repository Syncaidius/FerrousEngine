#include "file.h"
#include "stream_file.h"
using namespace std;

FileStream::FileStream(const FeString& path,
	const FileStreamFlags flags,
	bool canRead,
	bool canWrite,
	const UtfEncoding encoding) : 
	Stream(canRead, canWrite), _path(path){

	if (!File::exists(path)) {
		if ((flags & FileStreamFlags::Create) != FileStreamFlags::Create)
			throw FileNotFoundError(path.getData());
		else
			File::create(path);
	}

	_flags = flags;
	_encoding = encoding;

	int mode = 0;
	if (canRead)
		mode |= ios::in;

	if (_canWrite) {
		mode |= ios::out;

		if ((flags & FileStreamFlags::Append) == FileStreamFlags::Append)
			mode |= ios::app;
		else
			mode |= ios::trunc;
	}

	if ((flags & FileStreamFlags::Binary) == FileStreamFlags::Binary)
		mode |= ios::binary;

	_stream = fstream(path.getData(), mode);

	// Do we need to write a UTF byte order mark (BOM)?
	if (_canWrite) {
		if ((flags & FileStreamFlags::Append) != FileStreamFlags::Append) {
			if ((flags & FileStreamFlags::Binary) != FileStreamFlags::Binary) {
				const char* bom = UtfString::UTF_BOM[(uint8_t)encoding];
				if (bom[0] > 0)
					_stream.write(bom + 1, bom[0]);
			}
		}
	}

	if (_canRead) {
		// Read the BOM anyway, even if we're overriding it with an explicit encoding.
		// This ensures the BOM (if present) cannot get in the way when reading file data.
		UtfEncoding detectedEncoding = detectEncoding();
		if (encoding == UtfEncoding::Auto)
			_encoding = detectedEncoding;
	}
}

UtfEncoding FileStream::detectEncoding() {
	int bom_failed[UtfString::UTF_BOM_COUNT] = { 0 };

	size_t prevPos = _stream.tellg();

	// The biggest supported BOM is 3-bytes.
	for (int b = 1; b < 4; b++) {
		char bom_byte = 0;
		_stream.read(&bom_byte, 1);

		for (int i = 1; i < UtfString::UTF_BOM_COUNT; i++) {
			if (bom_failed[i])
				continue;

			if (i > UtfString::UTF_BOM[i][0]) {
				bom_failed[i] = true;
				continue;
			}

			if (UtfString::UTF_BOM[i][b] == bom_byte) {
				if (b == UtfString::UTF_BOM[i][0])
					return (UtfEncoding)i;
			}
			else {
				bom_failed[i];
			}
		}
	}

	// No supported BOM was found. Revert the stream back to it's last known position.
	_stream.seekg(prevPos, ios_base::beg);
	return UtfEncoding::UTF8;
}

FileStream::~FileStream() {
	close();
}

UtfEncoding FileStream::getEncoding() const {
	return _encoding;
}

void FileStream::close() {
	if (!_isOpen)
		return;

	_isOpen = false;
	_stream.close();
	//_stream.~basic_fstream();
}

size_t FileStream::getSize() {
	if (!_isOpen)
		throw StreamClosedError(this);

	// TODO Perhaps this can be retrieved faster via std::filesystem?

	// Use use whichever position we have access to.
	size_t num_bytes = 0;
	if (canRead()) {
		size_t curPos = _stream.tellg();
		_stream.seekg(0, ios_base::end);
		num_bytes = _stream.tellg();
		_stream.seekg(curPos, ios_base::beg);
	}
	else if (canWrite()) {
		size_t curPos = _stream.tellp();
		_stream.seekp(0, ios_base::end);
		num_bytes = _stream.tellp();
		_stream.seekp(curPos, ios_base::beg);
	}

	return num_bytes;
}

void FileStream::setSize(size_t num_bytes) {
	if (!_isOpen)
		throw StreamClosedError(this);

	// TODO Perhaps this can be done faster via std::filesystem?

	if (!_canWrite)
		throw StreamWriteAccessError(this);

	size_t curPos = _stream.tellp();
	_stream.seekp(num_bytes - 1, ios_base::beg);
	_stream.write("", 1);
	_stream.seekp(curPos, ios_base::beg);
}

size_t FileStream::getReadPos() {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canRead)
		throw StreamReadAccessError(this);

	return _stream.tellg();
}

void FileStream::setReadPos(size_t pos) {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canRead)
		throw StreamReadAccessError(this);

	_stream.seekg(pos, ios_base::beg);
}

void FileStream::seekRead(StreamSeek origin, int32_t num_bytes) {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canRead)
		throw StreamReadAccessError(this);

	int o = ios_base::beg;
	if (origin == StreamSeek::Current)
		o = ios_base::cur;
	else if (origin == StreamSeek::End)
		o = ios_base::end;

	_stream.seekg(num_bytes, o);
}

size_t FileStream::getWritePos() {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canWrite)
		throw StreamWriteAccessError(this);

	return _stream.tellp();
}

void FileStream::setWritePos(size_t pos) {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canWrite)
		throw StreamWriteAccessError(this);

	_stream.seekp(pos, ios_base::beg);
}

void FileStream::seekWrite(StreamSeek origin, int32_t num_bytes) {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canWrite)
		throw StreamWriteAccessError(this);

	int o = ios_base::beg;
	if (origin == StreamSeek::Current)
		o = ios_base::cur;
	else if (origin == StreamSeek::End)
		o = ios_base::end;

	_stream.seekp(num_bytes, o);
}

istream& FileStream::getReadStream() {
	return _stream;
}

ostream& FileStream::getWriteStream() {
	return _stream;
}

void FileStream::readBytes(char* dest, size_t num_bytes) {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canRead)
		throw StreamWriteAccessError(this);

	_stream.read(dest, num_bytes);
}

void FileStream::writeBytes(const char* data, size_t num_bytes) {
	if (!_isOpen)
		throw StreamClosedError(this);

	if (!_canWrite)
		throw StreamWriteAccessError(this);

	_stream.write(data, num_bytes);
}