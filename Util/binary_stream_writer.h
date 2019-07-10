#pragma once
#include "stream_writer.h"

class FERROUS_UTIL_API BinaryStreamWriter : StreamWriter {
public:
	BinaryStreamWriter(Stream* stream);
	virtual ~BinaryStreamWriter();

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
private:
	template<typename T>
	inline void write(const T* value, uint32_t count) {
		_stream->writeBytes(reinterpret_cast<const char*>(value), sizeof(T) * count);
	}
};

