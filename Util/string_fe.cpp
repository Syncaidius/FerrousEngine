#include "stdafx.h"
#include "string_fe.h"
#include "memory.h"
#include "localization.h"

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
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);

	for (size_t i = 0; i < _length; i++)
		new_data[i] = std::tolower(_data[i], *Localization::get()->getCurrentLocale());

	// TODO use default std locale instead of assuming we're using english.
	// TODO add overload to accept alternative locale
	// SEE: https://en.cppreference.com/w/cpp/locale/locale

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

FeString FeString::toUpper() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);

	for (size_t i = 0; i < _length; i++)
		new_data[i] = std::toupper(_data[i], *Localization::get()->getCurrentLocale());

	// TODO use default std locale instead of assuming we're using english.
	// TODO add overload to accept alternative locale
	// SEE: https://en.cppreference.com/w/cpp/locale/locale

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

