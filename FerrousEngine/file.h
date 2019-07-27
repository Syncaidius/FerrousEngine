#pragma once
#include "strings.h"
#include <filesystem>

#define DEFINE_FILE_ERROR(ClassName, Message) \
	class ClassName : public std::exception { \
		const wchar_t* _path; \
		public: \
		inline ClassName(const wchar_t* path, const char* msg = Message) : exception(msg) { \
			_path = path; \
		} \
		inline const wchar_t* getFile() const throw() { return _path; } \
	};

DEFINE_FILE_ERROR(FileAlreadyExistsError, "File already exists.");
DEFINE_FILE_ERROR(FileNotFoundError, "File not found.");
DEFINE_FILE_ERROR(FileRemovalError, " An unknown error occurred while attempting to remove a file.");
DEFINE_FILE_ERROR(NotDirectoryError, "Directory expected, but path is not a directory.");
DEFINE_FILE_ERROR(NotAFileError, "Directory expected, but path is not a directory.");
DEFINE_FILE_ERROR(DirectoryNotEmptyError, "Attempt to delete directory failed. Not empty.");
DEFINE_FILE_ERROR(DirectoryRemovalError, " An unknown error occurred while attempting to remove a directory.");
DEFINE_FILE_ERROR(DirectoryNotFoundError, "Directory not found.");

class File {
private:


public:
	static bool exists(const wchar_t* path);
	inline static bool exists(const FeString& path) {
		return exists(path.getData());
	}

	static bool isDirectory(const wchar_t* path);
	inline static bool isDirectory(const FeString& path) {
		return isDirectory(path.getData());
	}

	static bool isFile(const wchar_t* path);

	inline static bool isFile(const FeString* path) {
		return isFile(path->getData());
	}

	static FeString getWorkingDirectory() {
		return FeString(_info->working_dir.c_str(), Memory::get()); // TODO cache this as an FeString
	}

	/* Deletes a file if it exists.*/
	static void deleteFile(const wchar_t* path);
	inline static void deleteFile(const FeString* path) {
		deleteFile(path->getData());
	}

	/* Deletes a directory. If recursive is false, the folder must be empty before it can be deleted.*/
	static void deleteDirectory(const wchar_t* path, bool recursive);
	inline static void deleteDirectory(const FeString* path, bool recursive) {
		deleteDirectory(path->getData(), recursive);
	}

	static void create(const wchar_t* path);
	inline static void create(const FeString& path) {
		return create(path.getData());
	}

private:


	struct GlobalInfo {
		GlobalInfo();
		std::filesystem::path working_dir;
	};

	static GlobalInfo* _info;

public:

};
