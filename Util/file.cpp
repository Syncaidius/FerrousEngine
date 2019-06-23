#include "file.h"
#include "allocation.h"

using namespace std;
namespace fs = filesystem;

#pragma region Static
bool File::exists(const wchar_t* path) {
	return fs::exists(path);
}

bool File::isDirectory(const wchar_t* path) {
	return fs::is_directory(path);
}

bool File::isFile(const wchar_t* path) {
	return !fs::is_directory(path);
}

void File::deleteFile(const wchar_t* path) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		throw NotFoundError();

	if (fs::is_directory(p))
		throw NotAFileError();

	if (!fs::remove(p))
		throw RemovalError();
}

void File::deleteDirectory(const wchar_t* path, bool recursive) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		throw DirectoryNotFoundError();

	if (!fs::is_directory(p))
		throw NotDirectoryError();

	if (recursive) {
		if (fs::remove_all(p) <= 0)
			throw RemovalError();
	}
	else {
		if (!fs::is_empty(p))
			throw DirectoryNotEmptyError();

		if (!fs::remove(p))
			throw RemovalError();
	}
}

void File::create(const wchar_t* path) {
	if (fs::exists(path))
		throw AlreadyExistsError();

	fstream stream = fstream(path);
	stream.close();
}

#pragma endregion

#pragma region Instanced

File::File(const FeString path, const AccessFlags access, const ModeFlags mode, FerrousAllocator* allocator, const UtfEncoding encoding, size_t bufferSize) {
	if (!fs::exists(path.c_str())) {
		if ((mode & ModeFlags::Create) != ModeFlags::Create)
			throw NotFoundError();
		else
			create(path.c_str());
	}
	else {
		if (_isOpen)
			close();
	}

	_allocator = allocator;
	_access = access;
	_mode = mode;
	_encoding = encoding;
	_isOpen = true;

	int modeFlags = 0;
	if ((access & AccessFlags::Read) == AccessFlags::Read) 
		modeFlags |= ios::in;

	if ((access & AccessFlags::Write) == AccessFlags::Write) 
		modeFlags |= ios::out;

	if ((mode & ModeFlags::Binary) == ModeFlags::Binary)
		modeFlags |= ios::binary;

	if ((mode & ModeFlags::Append) == ModeFlags::Append)
		modeFlags |= ios::app;
	else
		modeFlags |= ios::trunc;

	_stream = fstream(path.c_str(), modeFlags);

	// Do we need to write a UTF byte order mark (BOM)?
	if (canWrite()) {
		if ((mode & ModeFlags::Append) != ModeFlags::Append) {
			if ((mode & ModeFlags::Binary) != ModeFlags::Binary) {
				const char* bom = UtfString::UTF_BOM[(uint8_t)encoding];
				if (bom[0] > 0)
					_stream.write(bom + 1, bom[0]);
			}
		}
	}

	_buffer = static_cast<char*>(_allocator->alloc(bufferSize));
}

File::~File() {
	_stream.close();
	_allocator->dealloc(_buffer);
}

void File::getPos(File::AccessFlags access, size_t& pos) {
	if (!_isOpen)
		throw NotOpenError();

	switch (access) {
	case AccessFlags::Read:
		pos = _stream.tellg();
		break;

	case AccessFlags::Write:
		pos = _stream.tellp();
		break;

	default:
		throw InvalidAccessFlagsError(this);
	}
}

void File::setPos(File::AccessFlags access, size_t pos) {
	if (!_isOpen)
		throw NotOpenError();

	switch (access) {
	case AccessFlags::Read:
		_stream.seekg(pos, ios_base::beg);
		break;

	case AccessFlags::Write:
		_stream.seekp(pos, ios_base::beg);
		break;

	default:
		throw InvalidAccessFlagsError(this);
	}
}

void File::seek(File::AccessFlags access, File::SeekOrigin origin, int32_t num_bytes) {
	if (!_isOpen)
		throw NotOpenError();

	int o = ios_base::beg;
	if (origin == SeekOrigin::Current)
		o = ios_base::cur;
	else if (origin == SeekOrigin::End)
			o = ios_base::end;

	switch (access) {
	case AccessFlags::Read:
		_stream.seekg(num_bytes, o);
		break;

	case AccessFlags::Write:
		_stream.seekp(num_bytes, o);
		break;

	default:
		throw InvalidAccessFlagsError(this);
	}
}

void File::getSize(size_t& num_bytes) {
	if (!_isOpen)
		throw NotOpenError();

	// TODO Perhaps this can be retrieved faster via std::filesystem?

	// Use use whichever position we have access to.
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
}

void File::setSize(size_t num_bytes) {
	if (!_isOpen)
		throw NotOpenError();

	// TODO Perhaps this can be done faster via std::filesystem?

	if (canWrite()) {
		size_t curPos = _stream.tellp();
		_stream.seekp(num_bytes - 1, ios_base::beg);
		_stream.write("", 1);
		_stream.seekp(curPos, ios_base::beg);
	}
}

void File::close() {
	if (!_isOpen)
		throw NotOpenError();

	_stream.close();
	_isOpen = false;
}

const bool File::canRead() {
	return (_access & AccessFlags::Read) == AccessFlags::Read;
}

const bool File::canWrite() {
	return (_access & AccessFlags::Write) == AccessFlags::Write;
}

void File::readBytes(char* dest, size_t num_bytes) {
	if (!_isOpen)
		throw NotOpenError();

	if (!canRead())
		throw ReadAccessError(this);

	_stream.read(dest, num_bytes);
}

void File::writeBytes(const char* data, size_t num_bytes) {
	if (!_isOpen)
		throw NotOpenError();

	if (!canWrite())
		throw WriteAccessError(this);

	_stream.write(data, num_bytes);
}

size_t File::readString(FeString* dest, uint32_t count) {
	if (!_isOpen)
		throw NotOpenError();

	if (!canRead())
		throw WriteAccessError(this);

	for (uint32_t i = 0; i < count; i++) {
		
	}
	return 0;
}

void File::writeString(const FeString* val, uint32_t count) {
	if (!_isOpen)
		throw NotOpenError();

	if (!canWrite())
		throw WriteAccessError(this);

	for (uint32_t i = 0; i < count; i++) {
		UtfString utf = UtfString::encode(val, _encoding);
		writeBytes(utf.getData(), utf.byteLen());
	}
	
}
#pragma endregion