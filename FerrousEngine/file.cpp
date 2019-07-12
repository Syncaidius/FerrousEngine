#include "file.h"
#include "allocation.h"
#include <fstream>

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
		throw FileNotFoundError(path);

	if (fs::is_directory(p))
		throw NotAFileError(path);

	if (!fs::remove(p))
		throw FileNotFoundError(path);
}

void File::deleteDirectory(const wchar_t* path, bool recursive) {
	auto p = fs::path(path);

	if (!fs::exists(p))
		throw DirectoryNotFoundError(path);

	if (!fs::is_directory(p))
		throw NotDirectoryError(path);

	if (recursive) {
		if (fs::remove_all(p) <= 0)
			throw DirectoryRemovalError(path);
	}
	else {
		if (!fs::is_empty(p))
			throw DirectoryNotEmptyError(path);

		if (!fs::remove(p))
			throw DirectoryRemovalError(path);
	}
}

void File::create(const wchar_t* path) {
	if (fs::exists(path))
		throw FileAlreadyExistsError(path);

	fstream stream = fstream(path);
	stream.close();
}

#pragma endregion