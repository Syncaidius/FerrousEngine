#include "stdafx.h"
#include "FeString.h"
#include "memory.h"

FeString::FeString() {
	_data = Memory::get()->alloc<wchar_t>();
	_length = 0;
}

/* Private constructor.*/
FeString::FeString(void* mem, wchar_t* c_data, size_t length) {
	_length = length;
	_data = c_data;
	_allocated = mem;
}

FeString::FeString(const char* c_data) {
	_length = strlen(c_data);
	_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);
	_allocated = _data;

	for (int i = 0; i < _length; i++)
		_data[i] = c_data[i];

	_data[_length] = L'\0';
}

FeString::FeString(const wchar_t* c_data) {
	_length = wcslen(c_data);
	_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);
	_allocated = _data;
	memcpy_s(_data, _length * sizeof(wchar_t), c_data, _length * sizeof(wchar_t));
	_data[_length] = L'\0';
}

FeString::~FeString() {
	Memory::get()->dealloc(_allocated);
}

