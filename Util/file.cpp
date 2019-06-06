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
		return NotFound;

	if (fs::is_directory(p))
		return NotFile;

	return fs::remove(p) ? Success : UnknownError;
}

File::FileResult File::deleteDirectory(const wchar_t* path, bool recursive) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		return NotFound;

	if (!fs::is_directory(p))
		return NotDirectory;

	if (recursive) {
		return fs::remove_all(p) > 0 ? Success : UnknownError;
	}
	else {
		if (!fs::is_empty(p))
			return NotEmpty;

		return fs::remove(p) ? Success : UnknownError;
	}
}