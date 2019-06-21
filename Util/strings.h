#pragma once
#include "stdafx.h"
#include "allocation.h"
#include <iostream>
#include <cstdarg>

enum struct UtfEncoding : uint8_t {
	/* UTF-8, no BOM */
	UTF8 = 0,

	/* UTF-8, with BOM */
	UTF8_WithBOM = 1,

	/* UTF-16 Big Endian (BE) */
	UTF16_BE = 2,

	/* UTF-16 Little Endian (LE).*/
	UTF16_LE = 3,
};

class FERROUS_UTIL_API FeString {
public:
	/* The value returned by indexOf() if no instance was found. */
	const static size_t INDEXOF_NONE = SIZE_MAX;

	static FeString format(const FeString& str, FerrousAllocator* allocator, ...);
	static FeString format(const FeString& str, ...);

	static FeString format(const FeString& str, FerrousAllocator* allocator, va_list args);
	static FeString format(const FeString& str, va_list args);

	/* Returns a new FeString containing the current date and/or time. Uses wcsftime formatting rules. See: https://en.cppreference.com/w/cpp/chrono/c/wcsftime */
	static FeString dateTime(const wchar_t* format, FerrousAllocator* allocator);
	inline static FeString dateTime(const wchar_t* format) {
		return dateTime(format, Memory::get());
	}

	/* Returns a new FeString containing the current date and/or time. Uses wcsftime formatting rules. See: https://en.cppreference.com/w/cpp/chrono/c/wcsftime */
	inline static FeString dateTime(const FeString& format, FerrousAllocator* allocator) {
		return dateTime(format.c_str(), allocator);
	}

	inline static FeString dataTime(const FeString& format) {
		return dateTime(format.c_str(), Memory::get());
	}

	FeString();
	FeString(FerrousAllocator* allocator);
	FeString(const FeString& copy);
	FeString(const char* c_data, FerrousAllocator* allocator);
	FeString(const wchar_t* c_data, FerrousAllocator* allocator);
	FeString(const char32_t* c_data, FerrousAllocator* allocator);
	inline FeString(const char* c_data) : FeString(c_data, Memory::get()) {}
	inline FeString(const wchar_t* c_data) : FeString(c_data, Memory::get()) {}
	inline FeString(const char32_t* c_data) : FeString(c_data, Memory::get()) {}
	~FeString();

	FeString substr(const size_t startIndex);
	FeString substr(const size_t startIndex, const size_t count);

	FeString replace(const wchar_t c, const wchar_t replacement);
	FeString replace(const FeString str, const FeString replacement);

	bool startsWith(const wchar_t c);
	bool startsWith(const FeString* input);

	bool endsWith(const wchar_t input);
	bool endsWith(const FeString* input);

	bool contains(const wchar_t c);
	bool contains(const FeString* input);

	/* Returns the index of the first instance of c. Returns SIZE_MAX if none were found.*/
	size_t indexOf(const wchar_t c);

	/* Returns the index of the first instance of input. Returns SIZE_MAX if none were found.*/
	size_t indexOf(const FeString* input);

	/* Returns a new string with the whitespace trimmed from the start and end.*/
	FeString trim();

	/* Returns a new string with the whitespace trimmed from the start.*/
	FeString trimStart();

	/* Returns a new string with the whitespace trimmed from the end.*/
	FeString trimEnd();

	/* Returns a new string with all characters in lower-case, where applicable.*/
	FeString toLower();

	/* Returns a new string with all characters in upper-case, where applicable.*/
	FeString toUpper();

	/* Capitalizes the first non-whitespace character in the string and returns a new string.*/
	FeString capitalizeFirst();

	/* Capitalizes the first character of each word in the string and returns a new string.*/
	FeString capitalize();

	inline FerrousAllocator* getAllocator() const { return _allocator; }

	/* Gets the length of the string.*/
	const inline size_t len() const { return _length; }

	/*Returns a pointer to the raw underlying character data.*/
	const inline wchar_t* c_str() const { return _data; }
private:
	/* Takes a pointer to character data and stores it, instead of allocating a separate copy of it. */
	FeString(wchar_t* c_data, size_t length, FerrousAllocator* allocator);

	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const FeString& b);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const uint8_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const uint16_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const uint32_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const uint64_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const int8_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const int16_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const int32_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const int64_t& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const double& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const long double& v);
	friend FeString FERROUS_UTIL_API operator +(const FeString& a, const float& v);

	friend FeString FERROUS_UTIL_API operator "" _fe(const char* c_data, size_t len);
	friend FeString FERROUS_UTIL_API operator "" _fe(const wchar_t* c_data, size_t len);
	friend FeString FERROUS_UTIL_API operator "" _fe(const char32_t* a, size_t len);

	wchar_t* _data;
	size_t _length;
	FerrousAllocator* _allocator;
};

class FERROUS_UTIL_API UtfString {
public:
	const static char UTF_BOM_8_NON[];
	const static char UTF_BOM_8[];
	const static char UTF_BOM_16_BE[];
	const static char UTF_BOM_16_LE[];
	const static char* UTF_BOM[];

	UtfString(uint32_t len, UtfEncoding encoding, FerrousAllocator* allocator);
	~UtfString();

	/* Re-encodes the current string to the specified encoding and returns the result as a byte array. */
	static UtfString encode(const FeString* string, UtfEncoding encoding, FerrousAllocator* allocator);
	inline static UtfString encode(const FeString* string, UtfEncoding encoding) {
		return encode(string, encoding, string->getAllocator());
	}

	/* Decodes a byte array of UTF data into a usable FeString and returns it.*/
	FeString decode(FerrousAllocator* allocator) const;
	FeString decode() const;

	inline const size_t byteLen() { return _num_bytes; }
	inline const uint32_t len() { return _len; }
	inline const char* getData() { return _data; }

private:
	const static uint8_t UTF8_LEAD_MASK[];
	const static uint8_t UTF8_LEAD_CAPACITY[];
	const static uint8_t UTF8_TRAIL_MASK;

	friend FERROUS_UTIL_API class FeString;
	char* _data;
	char* _mem;
	FerrousAllocator* _allocator;
	size_t _num_bytes;
	uint32_t _len;
	UtfEncoding _encoding;
};

#pragma region OPERATORS
FeString FERROUS_UTIL_API operator +(const FeString& a, const FeString& b);
FeString FERROUS_UTIL_API operator +(const FeString& a, const uint8_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const uint16_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const uint32_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const uint64_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const int8_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const int16_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const int32_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const int64_t& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const double& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const long double& v);
FeString FERROUS_UTIL_API operator +(const FeString& a, const float& v);

FeString FERROUS_UTIL_API operator "" _fe(const char* a, size_t len);
FeString FERROUS_UTIL_API operator "" _fe(const wchar_t* a, size_t len);
FeString FERROUS_UTIL_API operator "" _fe(const char32_t* a, size_t len);
#pragma endregion