#include "file.h"
#include "allocation.h"

namespace fs = std::filesystem;

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

	auto stream = std::fstream(path);
	stream.close();
	return Result::Success;
}

#pragma endregion

#pragma region Instanced

File::File(const wchar_t* path, const AccessFlags access, const ModeFlags mode) {
	int modeFlags = 0;
	if ((access & AccessFlags::Read) == AccessFlags::Read) 
		modeFlags |= std::ios::in;

	if ((access & AccessFlags::Write) == AccessFlags::Write) 
		modeFlags |= std::ios::out;

	if ((mode & ModeFlags::Binary) == ModeFlags::Binary)
		modeFlags |= std::ios::binary;

	if ((mode & ModeFlags::Append) == ModeFlags::Append)
		modeFlags |= std::ios::app;

	if ((mode & ModeFlags::Truncate) == ModeFlags::Truncate)
		modeFlags |= std::ios::trunc;

	_stream = std::fstream(path, modeFlags);
}

File::~File() {
	_stream.close();
	Memory::get()->dealloc(this);
}

size_t File::getPos(File::AccessFlags access) {

	switch (access) {
	case AccessFlags::Read:
		return _stream.tellg();

	case AccessFlags::Write:
		return _stream.tellp();

	default:
		throw Exception("Only Read or Write flags accepted.", Result::InvalidFlags);
	}

	return 0;
}

File::Result File::setPos(File::AccessFlags access, size_t pos) {
	switch (access) {
	case AccessFlags::Read:
		_stream.seekg(pos, std::ios_base::beg);

	case AccessFlags::Write:
		_stream.seekp(pos, std::ios_base::beg);

	default:
		throw Exception("Only Read or Write flags accepted.", Result::InvalidFlags);
	}

	return Result::UnknownError;
}

void File::close() {
	_stream.close();
}

#pragma endregion