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
	memcpy(_data, c_data, _length * sizeof(wchar_t));
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
		if (std::isspace(_data[i], loc)) {
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
		if (std::isspace(_data[i], loc)) {
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

FeString FeString::trim() {
	if (_length == 0)
		return FeString();

	std::locale loc = *Localization::get()->getCurrentLocale();
	size_t start = 0;
	size_t end = 0;

	for (start = 0; start < _length; start++) {
		if (!std::isspace(_data[start], loc))
			break;
	}

#pragma warning(push)
#pragma warning(disable: 6295) // _length cannot be zero at this point, so will not wrap or overflow to create an infinite loop.
	for (end = _length - 1; end >= 0; end--) {
		if (!std::isspace(_data[end], loc)) {
			end++;
			break;
		}
	}
#pragma warning(pop)

	size_t new_len = end - start;
	if (new_len == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(new_len + 1ULL);
	wchar_t* p_start = _data + start;
	memcpy(new_data, p_start, new_len * sizeof(wchar_t));

	new_data[new_len] = L'\0';
	return FeString(new_data, new_len);
}

FeString FeString::trimStart() {
	if (_length == 0)
		return FeString();

	std::locale loc = *Localization::get()->getCurrentLocale();
	size_t start = 0;

	for (start = 0; start < _length; start++) {
		if (!std::isspace(_data[start], loc))
			break;
	}

	size_t new_len = _length - start;
	if (new_len == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(new_len + 1ULL);
	wchar_t* p_start = _data + start;
	memcpy(new_data, p_start, new_len * sizeof(wchar_t));

	new_data[new_len] = L'\0';
	return FeString(new_data, new_len);
}

FeString FeString::trimEnd() {
	if (_length == 0)
		return FeString();

	std::locale loc = *Localization::get()->getCurrentLocale();
	size_t new_len = 0;

#pragma warning(push)
#pragma warning(disable: 6295) // _length cannot be zero at this point, so will not wrap or overflow to create an infinite loop.
	for (new_len = _length - 1; new_len >= 0; new_len--) {
		if (!std::isspace(_data[new_len], loc)) {
			new_len++;
			break;
		}
	}
#pragma warning(pop)

	if (new_len == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->alloc<wchar_t>(new_len + 1ULL);
	memcpy(new_data, _data, new_len * sizeof(wchar_t));

	new_data[new_len] = L'\0';
	return FeString(new_data, new_len);
}

size_t FeString::indexOf(const wchar_t c) {
	for (size_t i = 0; i < _length; i++) {
		if (_data[i] == c)
			return i;
	}

	return INDEXOF_NONE;
}

size_t FeString::indexOf(const FeString* input) {
	assert(input != nullptr);

	for (size_t i = 0; i < _length; i++) {
		if (_data[i] == input->_data[0]) {
			/* Already checked first char. */
			size_t j = 1;

			/* Now iterate over _data from i to see if the rest of the string matches. */
			for (; j < input->_length; j++) {
				if ((i + j >= _length) || (_data[i + j] != input->_data[j]))
					break;
			}

			if (j == input->_length)
				return i;
		}
	}

	return INDEXOF_NONE;
}

