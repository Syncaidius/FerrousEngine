#include "file.h"
#include "allocation.h"
#include <fstream>

using namespace std;
namespace fs = filesystem;

namespace fe {
#pragma region Static
	bool File::exists(const FeString& path) {
		FerrousAllocator* allocator = path.getAllocator();
		char* p8 = allocator->allocType<char>(path.len() + 1ULL);
		path.toChar8(p8);
		bool result = fs::exists(p8);

		allocator->dealloc(p8);
		return result;
	}

	bool File::isDirectory(const FeString& path) {
		FerrousAllocator* allocator = path.getAllocator();
		char* p8 = allocator->allocType<char>(path.len() + 1ULL);
		path.toChar8(p8);
		bool result = fs::is_directory(p8);

		allocator->dealloc(p8);
		return result;

	}

	bool File::isFile(const FeString& path) {
		FerrousAllocator* allocator = path.getAllocator();
		char* p8 = allocator->allocType<char>(path.len() + 1ULL);
		path.toChar8(p8);
		bool result = !fs::is_directory(p8); // return true;

		allocator->dealloc(p8);
		return result;
	}

	void File::deleteFile(const FeString& path) {
		FerrousAllocator* allocator = path.getAllocator();
		char* p8 = allocator->allocType<char>(path.len() + 1ULL);
		path.toChar8(p8);
		auto p = fs::path(p8);

		if (!fs::exists(p))
			throw FileNotFoundError(path);

		if (fs::is_directory(p))
			throw NotAFileError(path);

		if (!fs::remove(p))
			throw FileNotFoundError(path);

		allocator->dealloc(p8);
	}

	void File::deleteDirectory(const FeString& path, bool recursive) {
		FerrousAllocator* allocator = path.getAllocator();
		char* p8 = allocator->allocType<char>(path.len() + 1ULL);
		path.toChar8(p8);
		auto p = fs::path(p8);

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

	void File::create(const FeString& path) {
		FerrousAllocator* allocator = path.getAllocator();
		char* p8 = allocator->allocType<char>(path.len() + 1ULL);
		path.toChar8(p8);

		if (fs::exists(p8))
			throw FileAlreadyExistsError(path.getData());

		fstream stream = fstream(p8);
		stream.close();

		allocator->dealloc(p8);
	}

#pragma endregion
}