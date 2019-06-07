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