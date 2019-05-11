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
	std::locale loc = *Localization::get()->getCurrentLocale();

	for (size_t i = 0; i < _length; i++)
		new_data[i] = std::tolower(_data[i], loc);

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

FeString FeString::toUpper() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();

	for (size_t i = 0; i < _length; i++)
		new_data[i] = std::toupper(_data[i], loc);

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

FeString FeString::capitalize() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();
	bool capitalize = true;

	for (size_t i = 0; i < _length; i++) {
		if (std::isspace(_data[i])) {
			capitalize = true;
			new_data[i] = _data[i];
		}
		else {
			if (capitalize) {
				new_data[i] = std::toupper(_data[i], loc);
				capitalize = false;
			}
			else {
				new_data[i] = _data[i];
			}
		}
	}

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

FeString FeString::capitalizeFirst() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();
	bool capitalize = true;

	/* Loop until we hit a non-whitespace character*/
	for (size_t i = 0; i < _length; i++) {
		if (std::isspace(_data[i])) {
			new_data[i] = _data[i];
		}
		else {
			if (capitalize) {
				new_data[i] = std::toupper(_data[i], loc);
				capitalize = false;
			}
			else {
				new_data[i] = _data[i];
			}
		}
	}

	new_data[_length] = L'\0';
	return FeString(new_data, _length);
}

