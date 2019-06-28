#pragma once
#include "util.h"
#include "strings.h"
#include <filesystem>
#include <fstream>

class FERROUS_UTIL_API File {
public:
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
	};

	static bool exists(const wchar_t* path);
	inline static bool exists(const FeString* path) {
		return exists(path->getData());
	}

	static bool isDirectory(const wchar_t* path);
	inline static bool isDirectory(const FeString* path) {
		return isDirectory(path->getData());
	}

	static bool isFile(const wchar_t* path);

	inline static bool isFile(const FeString* path) {
		return isFile(path->getData());
	}

	static FeString getWorkingDirectory() {
		return FeString(_info->working_dir.c_str());
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
	inline static void create(const FeString* path) {
		return create(path->getData());
	}

	void seek(AccessFlags flags, SeekOrigin origin, int32_t num_bytes);
	void setPos(AccessFlags flags, size_t pos);
	void getPos(AccessFlags flags, size_t& pos);

	void setSize(size_t size);
	void getSize(size_t& size);

	void readBytes(char* dest, size_t num_bytes);
	void writeBytes(const char* bytes, size_t num_bytes);

	~File();
	File(const FeString path,
		const AccessFlags access,
		const ModeFlags mode,
		FerrousAllocator* allocator, 
		const UtfEncoding encoding = UtfEncoding::UTF8,
		size_t bufferSize = 1024);
	inline File(const FeString path,
		const AccessFlags access,
		const ModeFlags mode,
		const UtfEncoding encoding = UtfEncoding::UTF8,
		size_t bufferSize = 1024) : File(path, access, mode, Memory::get(), encoding, bufferSize) {}

#pragma region Read Helpers
	inline void readU8(uint8_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readU16(uint16_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readU32(uint32_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readU64(uint64_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readI8(int8_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readI16(int16_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readI32(int32_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readI64(int64_t* dest, uint32_t count = 1) { read(dest, count); }
	inline void readFloat(float* dest, uint32_t count = 1) { read(dest, count); }
	inline void readDouble(double* dest, uint32_t count = 1) { read(dest, count); }
	void readString(FeString* dest);
#pragma endregion

#pragma region Write Helpers
	inline void writeU8(const uint8_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeU16(const uint16_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeU32(const uint32_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeU64(const uint64_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeI8(const int8_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeI16(const int16_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeI32(const int32_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeI64(const int64_t* val, uint32_t count = 1) { write(val, count); }
	inline void writeFloat(const float* val, uint32_t count = 1) { write(val, count); }
	inline void writeDouble(const double* val, uint32_t count = 1) { write(val, count); }
	void writeString(const FeString& val);
#pragma endregion

	void close();
	inline const bool isOpen() { return _isOpen; }
	const bool canRead();
	const bool canWrite();

	/* Gets the underlying file stream.*/
	inline std::fstream& getStream() { return _stream; }

private:
#define DEFINE_FILE_ERROR(ClassName, Message) \
	class ClassName : public std::exception { \
		const File* _file; \
		public: \
		inline ClassName(const File* file = nullptr, const char* msg = Message) : exception(msg) { \
			_file = file; \
		} \
		inline const File* getFile() const throw() { return _file; } \
	}; \

	DEFINE_FILE_ERROR(AlreadyExistsError, "File already exists.");
	DEFINE_FILE_ERROR(ReadAccessError, "Attempt to read from file failed. No read access. Check access flags.");
	DEFINE_FILE_ERROR(WriteAccessError, "Attempt to write to file failed. No write access. Check access flags.");
	DEFINE_FILE_ERROR(NotOpenError, "File not open or has already been closed.");
	DEFINE_FILE_ERROR(DirectoryNotFoundError, "Directory not found.");
	DEFINE_FILE_ERROR(NotFoundError, "File not found.");
	DEFINE_FILE_ERROR(NotDirectoryError, "Directory expected, but path is not a directory.");
	DEFINE_FILE_ERROR(NotAFileError, "Directory expected, but path is not a directory.");
	DEFINE_FILE_ERROR(DirectoryNotEmptyError, "Attempt to delete directory failed. Not empty.");
	DEFINE_FILE_ERROR(RemovalError, " An unknown error occurred while attempting to remove a file or directory.");
	DEFINE_FILE_ERROR(InvalidAccessFlagsError, "Invalid access flags: Only read or write flags are excepted (but not both).");
	struct GlobalInfo {
		GlobalInfo();
		std::filesystem::path working_dir;
	};

	static GlobalInfo* _info;

	template<typename T>
	inline void read(T* dest, uint32_t count) {
		readBytes(reinterpret_cast<char*>(dest), sizeof(T) * count);
	}

	template<typename T>
	inline void write(const T* value, uint32_t count) {
		writeBytes(reinterpret_cast<const char*>(value), sizeof(T) * count);
	}

	char* _buffer;
	AccessFlags _access;
	ModeFlags _mode;
	UtfEncoding _encoding;
	bool _isOpen;
	const FeString _path;
	std::fstream _stream;
	FerrousAllocator* _allocator;
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