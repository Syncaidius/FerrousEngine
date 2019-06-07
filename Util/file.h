#pragma once
#include "util.h"
#include "string_fe.h"
#include <filesystem>

class FERROUS_UTIL_API File {
public:
	enum class Result : uint8_t {
		NotFound = 0,
		Success = 1,
		NotFile = 2,
		NotDirectory = 3,
		NotEmpty = 4,

		UnknownError = 255,
	};

	enum class AccessFlags : uint8_t {
		None = 0,
		Read = 1,
		Write = 2,
	};

	enum class ModeFlags : uint8_t {
		None = 0,
		Open = 1,
		Create = 2,
		Append = 4,
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
	static Result deleteFile(const wchar_t* path);
	inline static Result deleteFile(const FeString* path) {
		return deleteFile(path->c_str());
	}

	/* Deletes a directory. If recursive is false, the folder must be empty before it can be deleted.*/
	static Result deleteDirectory(const wchar_t* path, bool recursive);
	inline static Result deleteDirectory(const FeString* path, bool recursive) {
		return deleteDirectory(path->c_str(), recursive);
	}

private:
	struct GlobalInfo {
		GlobalInfo();
		std::filesystem::path working_dir;
	};

	static GlobalInfo* _info;

	AccessFlags _access;
	ModeFlags _mode;
	size_t _pos;
	std::ostream _stream;
};

#pragma region Operators
inline File::AccessFlags operator | (File::AccessFlags l, File::AccessFlags r)
{
	using T = std::underlying_type_t <File::AccessFlags>;
	return static_cast<File::AccessFlags>(static_cast<T>(l) | static_cast<T>(r));
}

inline File::AccessFlags& operator |= (File::AccessFlags& l, File::AccessFlags r)
{
	l = l | r;
	return l;
}

inline File::ModeFlags operator | (File::ModeFlags l, File::ModeFlags r)
{
	using T = std::underlying_type_t <File::ModeFlags>;
	return static_cast<File::ModeFlags>(static_cast<T>(l) | static_cast<T>(r));
}

inline File::ModeFlags& operator |= (File::ModeFlags & l, File::ModeFlags r)
{
	l = l | r;
	return l;
}
#pragma endregion