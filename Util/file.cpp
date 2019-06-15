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

void File::open(
	const wchar_t* path,
	File*& file,
	AccessFlags access,
	ModeFlags mode) {

	if (!fs::exists(path)) {
		if ((mode & ModeFlags::Create) != ModeFlags::Create)
			throw NotFoundError();
		else
			create(path);
	}

	void* mem = Memory::get()->allocType<File>();
	file = new (mem) File(path, access, mode);
}

void File::create(const wchar_t* path) {
	if (fs::exists(path))
		throw AlreadyExistsError();

	fstream stream = fstream(path);
	stream.close();
}

#pragma endregion

#pragma region Instanced

File::File(const wchar_t* path, const AccessFlags access, const ModeFlags mode) {
	if (_isOpen)
		close();

	_access = access;
	_mode = mode;
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

	if ((mode & ModeFlags::Truncate) == ModeFlags::Truncate)
		modeFlags |= ios::trunc;

	_stream = fstream(path, modeFlags);
}

File::~File() {
	_stream.close();
	Memory::get()->dealloc(this);
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

#pragma endregion