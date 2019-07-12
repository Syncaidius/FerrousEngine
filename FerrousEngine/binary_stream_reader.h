#pragma once
#include "stream_reader.h"

class BinaryStreamReader : StreamReader {
public:
	BinaryStreamReader(Stream* stream, FerrousAllocator* allocator, size_t bufferSize = 1024);
	virtual ~BinaryStreamReader();

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

private:
	template<typename T>
	inline void read(T* dest, uint32_t count) {
		_stream->readBytes(reinterpret_cast<char*>(dest), sizeof(T) * count);
	}
};
