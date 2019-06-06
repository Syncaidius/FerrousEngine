#include "file.h"

bool File::exists(const wchar_t* path) {
	return std::filesystem::exists(path);
}

bool File::isDirectory(const wchar_t* path) {
	return std::filesystem::is_directory(path);
}

bool File::isFile(const wchar_t* path) {
	return !std::filesystem::is_directory(path);
}