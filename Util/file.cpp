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

File::Result File::deleteFile(const wchar_t* path) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		return Result::NotFound;

	if (fs::is_directory(p))
		return Result::NotFile;

	return fs::remove(p) ? Result::Success : Result::UnknownError;
}

File::Result File::deleteDirectory(const wchar_t* path, bool recursive) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		return Result::NotFound;

	if (!fs::is_directory(p))
		return Result::NotDirectory;

	if (recursive) {
		return fs::remove_all(p) > 0 ? Result::Success : Result::UnknownError;
	}
	else {
		if (!fs::is_empty(p))
			return Result::NotEmpty;

		return fs::remove(p) ? Result::Success : Result::UnknownError;
	}
}

File::Result File::open(
	const wchar_t* path,
	File*& file,
	AccessFlags access,
	ModeFlags mode) {

	Result result = Result::Success;

	if (!fs::exists(path)) {
		if ((mode & ModeFlags::Create) != ModeFlags::Create) {
			result = Result::NotFound;
		}
		else {
			result = create(path);
		}
	}

	if (result == Result::Success) {
		void* mem = Memory::get()->allocType<File>();
		file = new (mem) File(path, access, mode);
	}

	return result;
}

File::Result File::create(const wchar_t* path) {
	if (fs::exists(path))
		return Result::AlreadyExists;

	auto stream = fstream(path);
	stream.close();
	return Result::Success;
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

File::Result File::getPos(File::AccessFlags access, size_t& pos) {
	if (!_isOpen) {
		pos = 0;
		return Result::AlreadyClosed;
	}

	switch (access) {
	case AccessFlags::Read:
		pos = _stream.tellg();
		return Result::Success;

	case AccessFlags::Write:
		pos = _stream.tellp();
		return Result::Success;

	default:
		assert(false); // Only read or write flags are accepted. (Read | Write) combined is also not accepted.
		return Result::InvalidFlags;
	}
}

File::Result File::setPos(File::AccessFlags access, size_t pos) {
	if (!_isOpen)
		return Result::AlreadyClosed;

	switch (access) {
	case AccessFlags::Read:
		_stream.seekg(pos, ios_base::beg);
		return Result::Success;
		break;

	case AccessFlags::Write:
		_stream.seekp(pos, ios_base::beg);
		return Result::Success;
		break;

	default:
		assert(false); // Only read or write flags are accepted. (Read | Write) combined is also not accepted.
		return Result::InvalidFlags;
	}

	return Result::UnknownError;
}

File::Result File::seek(File::AccessFlags access, File::SeekOrigin origin, int32_t num_bytes) {
	if (!_isOpen)
		return Result::AlreadyClosed;

	int o = ios_base::beg;
	if (origin == SeekOrigin::Current)
		o = ios_base::cur;
	else if (origin == SeekOrigin::End)
			o = ios_base::end;

	switch (access) {
	case AccessFlags::Read:
		_stream.seekg(num_bytes, o);
		return Result::Success;
		break;

	case AccessFlags::Write:
		_stream.seekp(num_bytes, o);
		return Result::Success;
		break;

	default:
		assert(false); // Only read or write flags are accepted. (Read | Write) combined is also not accepted.
		return Result::InvalidFlags;
	}
}

File::Result File::close() {
	if (!_isOpen)
		return Result::AlreadyClosed;

	_stream.close();
	_isOpen = false;
	return Result::Success;
}

#pragma endregion