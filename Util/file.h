#pragma once
#include "util.h"
#include "string_fe.h"
#include <filesystem>

class FERROUS_UTIL_API File {
public:
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

private:
	struct GlobalInfo {
		GlobalInfo();
		std::filesystem::path working_dir;
	};

	static GlobalInfo* _info;
};