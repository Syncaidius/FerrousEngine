#include "stdafx.h"
#include "FeString.h"
#include "memory.h"

FeString::FeString() {
	_data = Memory::get()->alloc<wchar_t>();
	_length = 0;
}

/* Private constructor.*/
FeString::FeString(wchar_t* c_data, size_t length) {
	_length = length;
	_data = c_data;
}

FeString::FeString(const char* c_data) {
	_length = strlen(c_data);
	_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);

	for (size_t i = 0; i < _length; i++)
		_data[i] = c_data[i];

	_data[_length] = L'\0';
}

FeString::FeString(const wchar_t* c_data) {
	_length = wcslen(c_data);
	_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);
	memcpy_s(_data, _length * sizeof(wchar_t), c_data, _length * sizeof(wchar_t));
	_data[_length] = L'\0';
}

FeString::~FeString() {
	Memory::get()->dealloc(_data);
}

FeString FeString::toLower() {
	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);

	for (size_t i = 0; i < _length; i++) {
		if (_length > 0 && _data[i] >= 'A' && _data[i] <= 'Z') 
			new_data[i] += _data[i] + 32;
		else 
			new_data[i] = _data[i];
	}

	// TODO use default std locale instead of assuming we're using english.
	// TODO add overload to accept alternative locale

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

FeString FeString::toUpper() {
	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);

	for (size_t i = 0; i < _length; i++) {
		if (_length > 0 && _data[i] >= 'a' && _data[i] <= 'z')
			new_data[i] += _data[i] - 32;
		else
			new_data[i] = _data[i];
	}

	// TODO use default std locale instead of assuming we're using english.
	// TODO add overload to accept alternative locale

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

