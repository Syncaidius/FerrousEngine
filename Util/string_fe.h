#pragma once
#include "stdafx.h"
#include "memory.h"
#include <iostream>

class FERROUS_UTIL_API FeString {
public:
	static FeString concat(const FeString& a, const FeString& b);
	static FeString format(const FeString& str, ...);

	FeString();
	FeString(const char* c_data);
	FeString(const wchar_t* c_data);
	~FeString();

	FeString substr(const uint32_t startIndex);
	FeString substr(const uint32_t startIndex, const uint32_t count);
	FeString replace(const char* c_data);
	FeString replace(const char c_data);
	FeString replace(const wchar_t* c_data);
	FeString replace(const wchar_t c_data);
	FeString replace(const FeString* str);

	bool startsWith(const FeString* input);
	bool endsWith(const FeString* sinputtr);
	bool contains(const FeString* input);
	uint32_t indexOf(const char* input);
	uint32_t indexOf(const wchar_t* input);
	uint32_t indexOf(const char c);
	uint32_t indexOf(const wchar_t c);
	uint32_t indexOf(const FeString* input);

	FeString trim();
	FeString trimStart();
	FeString trimEnd();
	FeString toLower();
	FeString toUpper();

	/* Capitalizes the first character of the string.*/
	void capitalize();

	/* Capitalizes the first letter of every word in the string.*/
	void capitalizeAll();

	/* Gets the length of the string.*/
	uint32_t getLength() { return _length; }

	/*Returns a pointer to the raw underlying character data.*/
	wchar_t* c_str() { return _data; }
private:
	FeString(wchar_t* c_data, size_t length);
	friend FeString operator +(const FeString& a, const FeString& b);
	friend FeString operator "" _fe(const char* c_data, size_t len);
	wchar_t* _data;
	size_t _length;
};

FeString operator +(const FeString& a, const FeString& b) {
	size_t a_bytes = a._length * sizeof(wchar_t);
	size_t b_bytes = b._length * sizeof(wchar_t);
	size_t len = a._length + b._length;
	size_t num_bytes = a_bytes + b_bytes + sizeof(wchar_t);

	void* mem = Memory::get()->alloc(num_bytes);
	char* p_data = reinterpret_cast<char*>(mem);

	memcpy_s(p_data, a_bytes + b_bytes, a._data, a_bytes);
	p_data += a_bytes;
	memcpy_s(p_data, a_bytes + b_bytes, b._data, b_bytes);
	p_data += b_bytes;
	*p_data = L'\0';

	return FeString((wchar_t*)mem, len);
}

FeString operator "" _fe(const char* a, size_t len) {
	size_t a_bytes = len * sizeof(wchar_t);
	size_t num_bytes = a_bytes + sizeof(wchar_t);

	void* mem = Memory::get()->alloc(num_bytes);
	char* p_start = reinterpret_cast<char*>(mem);
	wchar_t* p_data = (wchar_t*)p_start;

	for (size_t i = 0; i < len; i++)
		p_data[i] = a[i];

	p_data += len;
	*p_data = L'\0';

	return FeString((wchar_t*)p_start, len);
}