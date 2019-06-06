#pragma once
#include "util.h"
#include "string_fe.h"
#include <filesystem>

class FERROUS_UTIL_API File {
public:
	enum FileResult : uint8_t {
		NotFound = 0,
		Success = 1,
		NotFile = 2,
		NotDirectory = 3,
		NotEmpty = 4,

		UnknownError = 255,
	};

	static bool exists(const wchar_t* path);
	inline static bool exists(const FeString* path) {
		return exists(path->c_str());
	}

	static bool isDirectory(const wchar_t* path);
	inline static bool isDirectory(const FeString* path) {
		return isDirectory(path->c_str());
	}

	static bool isFile(const wchar_t* path);

	inline static bool isFile(const FeString* path) {
		return isFile(path->c_str());
	}

	static FeString getWorkingDirectory() {
		return FeString(_info->working_dir.c_str());
	}

	/* Deletes a file if it exists.*/
	static FileResult deleteFile(const wchar_t* path);
	inline static FileResult deleteFile(const FeString* path) {
		return deleteFile(path->c_str());
	}

	/* Deletes a directory. If recursive is false, the folder must be empty before it can be deleted.*/
	static FileResult deleteDirectory(const wchar_t* path, bool recursive);
	inline static FileResult deleteDirectory(const FeString* path, bool recursive) {
		return deleteDirectory(path->c_str(), recursive);
	}

private:
	struct GlobalInfo {
		GlobalInfo();
		std::filesystem::path working_dir;
	};

	static GlobalInfo* _info;
};