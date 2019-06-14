#pragma once
#include "util.h"
#include "string_fe.h"
#include <filesystem>
#include <fstream>

class FERROUS_UTIL_API File {
public:
	enum class Result : uint8_t {
		/* File was not found.*/
		NotFound = 0,
		
		/* Operation succeeded.*/
		Success = 1,

		/* A file-based call was made on a non-file path.*/
		NotFile = 2,

		/* A directory-based call was made on a file path.*/
		NotDirectory = 3,

		/* The file or directory was not empty.*/
		NotEmpty = 4,

		/* The position was out of bounds. 
		This is usually because the file was shrunk below the currnet position, 
		or an attempt was made to set the position beyond the end of a file. */
		OutOfBounds = 5,

		/* Occurs when invalid flags were provided to a function. */
		InvalidFlags = 6,

		/* An attempt was made to create a file which already exists. */
		AlreadyExists = 7,

		/* An operation was attempted on a File but it was already closed.*/
		AlreadyClosed = 8,

		/* An unknown error occurred. */
		UnknownError = 255,
	};

	enum class SeekOrigin {
		/* From the beginning of the file. */
		Beginning = 0,

		/* From the current read or write position of the file.*/
		Current = 1,

		/* From the end of the file. */
		End = 2,
	};

	enum class AccessFlags : uint8_t {
		None = 0,
		Read = 1,
		Write = 2,
	};

	enum class ModeFlags : uint8_t {
		None = 0,

		/* Creates the file if it is missing, otherwise the existing file will be opened. */
		Create = 1,

		/* Once the file is opened, move to the end of the file to write new data.*/
		Append = 2,

		/* Open the file in binary mode. Default is text mode.*/
		Binary = 4,

		/* If the file is opened, it will be truncated to 0 bytes of data ready to */
		Truncate = 8,
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

	static Result open(const wchar_t* path, File*& file);
	inline static Result open(const FeString* path, File*& file) {
		return deleteDirectory(path->c_str(), file);
	}

	static Result open(
		const wchar_t* path, 
		File*& file, 
		AccessFlags access, 
		ModeFlags mode);
	inline static Result open(
		const FeString* path, 
		File*& file, 
		AccessFlags access, 
		ModeFlags mode) {
		return open(path->c_str(), file, access, mode);
	}

	static Result create(const wchar_t* path);
	inline static Result create(const FeString* path) {
		return create(path->c_str());
	}

	Result seek(AccessFlags flags, SeekOrigin origin, int32_t num_bytes);
	Result setPos(AccessFlags flags, size_t pos);
	Result getPos(AccessFlags flags, size_t& pos);

	Result setSize(size_t size);
	Result getSize(size_t& size);

	void readBytes(char* dest, size_t num_bytes);
	template<typename T>
	inline void read(T* dest) {
		readBytes(reinterpret_cast<char*>(dest), sizeof(T));
	}

	template<>
	inline void read<FeString>(FeString* dest) {
		size_t len;
		read<size_t>(&len);

		wchar_t* data = Memory::get()->allocType<wchar_t>(len + 1);
		readBytes(reinterpret_cast<char*>(data), sizeof(wchar_t) * (len + 1));
		new (dest) FeString(data, Memory::get());
	}

	void writeBytes(const char* bytes, size_t num_bytes);
	template<typename T>
	inline void write(const T& value) {
		writeBytes(reinterpret_cast<const char*>(value), sizeof(T));
	}

	template<>
	inline void write(const FeString& value) {
		size_t len = value.len();
		write(&len);
		const wchar_t* data = value.c_str();
		writeBytes(reinterpret_cast<const char*>(data), sizeof(wchar_t) * (len + 1));
	}

	template<typename T>
	inline void readArray(T* dest, size_t num_elements) {
		size_t num_bytes = num_elements * sizeof(T);
		readBytes(reinterpret_cast<char*>(dest), num_bytes);
	}

	template<typename T>
	inline void writeArray(const T* dest, size_t num_elements) {
		size_t num_bytes = num_elements * sizeof(T);
		writeBytes(reinterpret_cast<const char*>(dest), num_bytes);
	}

	Result close();
	inline const bool isOpen() { return _isOpen; }
	const bool canRead();
	const bool canWrite();

	/* Gets the underlying file stream.*/
	inline std::fstream& getStream() { return _stream; }
	
	~File();

private:
	struct GlobalInfo {
		GlobalInfo();
		std::filesystem::path working_dir;
	};

	static GlobalInfo* _info;

	File(const wchar_t* path, const AccessFlags access, const ModeFlags mode);

	AccessFlags _access;
	ModeFlags _mode;
	size_t _pos;
	bool _isOpen;
	const FeString _path;
	std::fstream _stream;
};

#pragma region Enum Operators
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

inline File::AccessFlags operator & (File::AccessFlags l, File::AccessFlags r)
{
	using T = std::underlying_type_t <File::AccessFlags>;
	return static_cast<File::AccessFlags>(static_cast<T>(l) | static_cast<T>(r));
}

inline File::AccessFlags& operator &= (File::AccessFlags & l, File::AccessFlags r)
{
	l = l & r;
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

inline File::ModeFlags operator & (File::ModeFlags l, File::ModeFlags r) {
	using T = std::underlying_type_t <File::ModeFlags>;
	return static_cast<File::ModeFlags>(static_cast<T>(l) & static_cast<T>(r));
}

inline File::ModeFlags& operator &= (File::ModeFlags& l, File::ModeFlags r)
{
	l = l & r;
	return l;
}

inline bool operator == (File::ModeFlags l, bool r) {
	using T = std::underlying_type_t <File::ModeFlags>;
	return static_cast<T>(l) > 0;
}

#pragma endregion