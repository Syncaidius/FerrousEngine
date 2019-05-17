#pragma once
#include "stdafx.h"
#include "memory.h"
#include <iostream>

class FERROUS_UTIL_API FeString {
public:
	/* The value returned by indexOf() if no instance was found. */
	const static size_t INDEXOF_NONE = SIZE_MAX;

	static FeString concat(const FeString& a, const FeString& b);
	static FeString format(const FeString& str, ...);

	FeString();
	FeString(const FeString& copy);
	FeString(const char* c_data);
	FeString(const wchar_t* c_data);
	~FeString();

	FeString substr(const size_t startIndex);
	FeString substr(const size_t startIndex, const size_t count);

	FeString replace(const wchar_t c, const wchar_t replacement);
	FeString replace(const FeString* str, const FeString* replacement);

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

	/* Gets the length of the string.*/
	size_t getLength() const { return _length; }

	/*Returns a pointer to the raw underlying character data.*/
	wchar_t* c_str() const { return _data; }
private:
	FeString(wchar_t* c_data, size_t length);
	friend FeString operator +(const FeString& a, const FeString& b);
	friend FeString operator "" _fe(const char* c_data, size_t len);
	friend FeString operator "" _fe(const wchar_t* c_data, size_t len);
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

	memcpy(p_data, a._data, a_bytes);
	p_data += a_bytes;
	memcpy(p_data, b._data, b_bytes);
	p_data += b_bytes;
	*p_data = L'\0';

	return FeString((wchar_t*)mem, len);
}

FeString operator "" _fe(const char* a, size_t len) {
	wchar_t* p = Memory::get()->allocType<wchar_t>(len + 1ULL);
	for (size_t i = 0; i < len; i++)
		p[i] = p[i];

	p[len] = L'\0';
	return FeString(p, len);
}

FeString operator "" _fe(const wchar_t* a, size_t len) {
	wchar_t* p = Memory::get()->allocType<wchar_t>(len + 1ULL);
	Memory::get()->copyType<wchar_t>(p, a, len);

	p[len] = L'\0';
	return FeString(p, len);
}