#include "file.h"

namespace fs = std::filesystem;

bool File::exists(const wchar_t* path) {
	return fs::exists(path);
}

bool File::isDirectory(const wchar_t* path) {
	return fs::is_directory(path);
}

bool File::isFile(const wchar_t* path) {
	return !fs::is_directory(path);
}

File::FileResult File::deleteFile(const wchar_t* path) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		return FileResult::NotFound;

	if (fs::is_directory(p))
		return FileResult::NotFile;

	return fs::remove(p) ? FileResult::Success : FileResult::UnknownError;
}

File::FileResult File::deleteDirectory(const wchar_t* path, bool recursive) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		return FileResult::NotFound;

	if (!fs::is_directory(p))
		return FileResult::NotDirectory;

	if (!fs::is_empty(p) && recursive)
		return FileResult::NotEmpty;

	return fs::remove(p) ? FileResult::Success : FileResult::UnknownError;
}