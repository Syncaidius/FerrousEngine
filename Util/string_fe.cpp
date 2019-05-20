#include "stdafx.h"
#include "string_fe.h"
#include "memory.h"
#include "localization.h"
#include <vector>

#pragma region STATIC
//FeString FeString::format(const FeString& str, ...) {
//
//}

FeString FeString::dateTime(const wchar_t* format) {
	static size_t buf_size = 80;
	static wchar_t* buf = Memory::get()->allocType<wchar_t>(buf_size); // Thread-safe due to each thread having its own version of the static.

	assert(format != nullptr);
	time_t now = time(nullptr);
	struct tm tstruct;
	localtime_s(&tstruct, &now);
	size_t len = wcsftime(buf, buf_size, format, &tstruct);
	if (len == 0) { // buffer was not big enough
		size_t old_size = buf_size;
		buf_size *= 2;

		Memory::get()->reallocType<wchar_t>(buf, old_size, buf_size);
		return dateTime(format);
	}

	wchar_t* mem = Memory::get()->allocTypeFast<wchar_t>(len + 1ULL);
	memcpy(mem, buf, len * sizeof(wchar_t));
	return FeString(mem, len);
}


template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type*>
FeString FeString::concat_number(const FeString & a, const T & v, const wchar_t* format) {
	static size_t buf_size = 10;
	static wchar_t* buf = Memory::get()->allocType<wchar_t>(buf_size); // Thread-safe due to each thread having its own version of the static.

	size_t len_v = _snwprintf(buf, buf_size, format, v);
	if (len_v < 0) {
		size_t old_size = buf_size;
		buf_size *= 2;

		Memory::get()->reallocType<wchar_t>(buf, old_size, buf_size);
		return concat_number(a, v, format);
	}

	size_t total_len = a._length + len_v;
	wchar_t* mem = Memory::get()->allocTypeFast<wchar_t>(total_len + 1ULL);
	wchar_t* p_data = mem;
	memcpy(p_data, a._data, a._length * sizeof(wchar_t));
	p_data += a._length;
	memcpy(p_data, buf, len_v * sizeof(wchar_t));

	return FeString(mem, total_len);
}
#pragma endregion

#pragma region INSTANCED
FeString::FeString() {
	_data = Memory::get()->allocType<wchar_t>();
	_length = 0;
}

FeString::FeString(const FeString& copy) {
	_data = copy._data;
	Memory::get()->ref(_data);
	_length = copy._length;
}

/* Private constructor.*/
FeString::FeString(wchar_t* c_data, size_t length) {
	_length = length;
	_data = c_data;
	_data[_length] = L'\0';
}

FeString::FeString(const char* c_data) {
	_length = strlen(c_data);
	_data = Memory::get()->allocType<wchar_t>(_length + 1ULL);

	for (size_t i = 0; i < _length; i++)
		_data[i] = c_data[i];

	_data[_length] = L'\0';
}

FeString::FeString(const wchar_t* c_data) {
	_length = wcslen(c_data);
	_data = Memory::get()->allocType<wchar_t>(_length + 1ULL);
	memcpy(_data, c_data, _length * sizeof(wchar_t));
	_data[_length] = L'\0';
}

FeString::~FeString() {
	Memory::get()->deref(_data);
}

FeString FeString::toLower() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();

	for (size_t i = 0; i < _length; i++)
		new_data[i] = std::tolower(_data[i], loc);

	return FeString(new_data, _length);
}

FeString FeString::toUpper() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();

	for (size_t i = 0; i < _length; i++)
		new_data[i] = std::toupper(_data[i], loc);

	return FeString(new_data, _length);
}

FeString FeString::capitalize() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(_length + 1ULL);
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

	return FeString(new_data, _length);
}

FeString FeString::capitalizeFirst() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(_length + 1ULL);
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

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(new_len + 1ULL);
	wchar_t* p_start = _data + start;
	memcpy(new_data, p_start, new_len * sizeof(wchar_t));

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

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(new_len + 1ULL);
	wchar_t* p_start = _data + start;
	memcpy(new_data, p_start, new_len * sizeof(wchar_t));
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

	wchar_t* new_data = Memory::get()->allocType<wchar_t>(new_len + 1ULL);
	memcpy(new_data, _data, new_len * sizeof(wchar_t));
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

	if (_length >= input->_length)
	{
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
	}

	return INDEXOF_NONE;
}

FeString FeString::substr(const size_t startIndex) {
	assert(startIndex < _length);

	size_t count = _length - startIndex;
	wchar_t* mem = Memory::get()->allocType<wchar_t>(count + 1ULL);
	wchar_t* src_pos = _data + startIndex;
	memcpy(mem, src_pos, count * sizeof(wchar_t));
	return FeString(mem, count);
}

FeString FeString::substr(const size_t startIndex, const size_t count) {
	assert((startIndex + count) < _length);

	wchar_t* mem = Memory::get()->allocType<wchar_t>(count + 1ULL);
	wchar_t* src_pos = _data + startIndex;
	Memory::get()->copy(mem, src_pos, count * sizeof(wchar_t));
	return FeString(mem, count);
}

FeString FeString::replace(const wchar_t c, const wchar_t replacement) {
	wchar_t* mem = Memory::get()->allocType<wchar_t>(_length + 1ULL);
	memcpy(mem, _data, (_length + 1ULL) * sizeof(wchar_t));

	for (size_t i = 0; i < _length; i++) {
		if (mem[i] == c)
			mem[i] = replacement;
	}

	return FeString(mem, _length);
}

FeString FeString::replace(const FeString* input, const FeString* replacement) {
	assert(input != nullptr);
	assert(replacement != nullptr);

	/* Is the string at least as big as the input*/
	if (_length < input->_length)
	{
		wchar_t* cpy = Memory::get()->allocType<wchar_t>(input->_length + 1ULL);
		memcpy(cpy, input->_data, input->_length + 1ULL);
		return FeString(cpy, input->_length);
	}

	std::vector<size_t> indices; // TODO: Check if this allocates cleanly. i.e. not dynamically allocating/deallocating via malloc/free.

	// Collect index of every match.
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
				indices.push_back(i);
		}
	}

	// Build new string
	size_t replacement_bytes = replacement->_length * sizeof(wchar_t);
	size_t count = indices.size();
	size_t new_len = _length;

	// TODO find a better way to handle negative input_dif, instead of branching.
	if (replacement->_length >= input->_length) {
		size_t input_dif = replacement->_length - input->_length;
		new_len = _length + (input_dif * count);
	}
	else { // Input is smaller than the replacement
		size_t input_dif = input->_length - replacement->_length;
		new_len = _length - (input_dif * count);
	}

	size_t prev_replace_end = 0; /* Ending of previous replacement.*/

	wchar_t* mem = Memory::get()->allocType<wchar_t>(new_len + 1ULL);
	wchar_t* mem_pos = mem;

	for (size_t i = 0; i < count; i++) {
		/*	Was there non-replaced data between the current and previous replacement?
			If so, copy it. */
		if (indices[i] != prev_replace_end) {
			size_t dif = indices[i] - prev_replace_end;
			memcpy(mem_pos, _data + prev_replace_end, dif * sizeof(wchar_t));
			mem_pos += dif;
		}

		/* Now copy the replacement string into new string.*/
		memcpy(mem_pos, replacement->_data, replacement_bytes);
		mem_pos += replacement->_length;
		prev_replace_end = indices[i] + input->_length;
	}

	assert(prev_replace_end <= _length);
	if (prev_replace_end < _length) {
		size_t dif = _length - prev_replace_end;
		memcpy(mem_pos, _data + prev_replace_end, dif * sizeof(wchar_t));
	}

	return FeString(mem, new_len);
}

bool FeString::contains(const wchar_t c) {
	for (size_t i = 0; i < _length; i++) {
		if (_data[i] == c)
			return true;
	}

	return false;
}

bool FeString::contains(const FeString* input) {
	return indexOf(input) != INDEXOF_NONE;
}

bool FeString::endsWith(const wchar_t c) {
	return _length > 0 && _data[_length - 1] == c;
}

bool FeString::endsWith(const FeString* input) {
	if (_length >= input->_length)
	{
		size_t start = _length - input->_length;

		for (size_t i = start; i < _length; i++) {
			if (_data[i] == input->_data[0]) {
				/* Already checked first char. */
				size_t j = 1;

				/* Now iterate over _data from i to see if the rest of the string matches. */
				for (; j < input->_length; j++) {
					if ((i + j >= _length) || (_data[i + j] != input->_data[j]))
						break;
				}

				if (j == input->_length)
					return true;
			}
		}
	}

	return false;
}

bool FeString::startsWith(const wchar_t c) {
	return _length > 0 && _data[0] == c;
}

bool FeString::startsWith(const FeString* input) {
	if (_length >= input->_length) {
		size_t i = 0;

		/* Iterate over input to see if all chars match. */
		for (; i < input->_length; i++) {
			if ((_data[i] != input->_data[i]))
				break;
		}

		if (i == input->_length)
			return true;
	}

	return false;
}
#pragma endregion

#pragma region OPERATORS
FeString operator +(const FeString& a, const FeString& b) {
	size_t len = a._length + b._length;

	wchar_t* mem = Memory::get()->allocType<wchar_t>(len + 1ULL);
	wchar_t* p_data = mem;

	memcpy(p_data, a._data, a._length * sizeof(wchar_t));
	p_data += a._length;
	memcpy(p_data, b._data, b._length * sizeof(wchar_t));

	return FeString(mem, len);
}

FeString operator +(const FeString& a, const uint8_t& v) { return FeString::concat_number<uint8_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const uint16_t& v) { return FeString::concat_number<uint16_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const uint32_t& v) { return FeString::concat_number<uint32_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const uint64_t& v) { return FeString::concat_number<uint64_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const int8_t& v) { return FeString::concat_number<int8_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const int16_t& v) { return FeString::concat_number<int16_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const int32_t& v) { return FeString::concat_number<int32_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const int64_t& v) { return FeString::concat_number<int64_t>(a, v, L"%d"); }
FeString operator +(const FeString& a, const double& v) { return FeString::concat_number<double>(a, v, L"%f"); }
FeString operator +(const FeString& a, const long double& v) { return FeString::concat_number<long double>(a, v, L"%f"); }
FeString operator +(const FeString& a, const float& v) { return FeString::concat_number<float>(a, v, L"%f"); }

FeString operator "" _fe(const char* a, size_t len) {
	wchar_t* p = Memory::get()->allocType<wchar_t>(len + 1ULL);
	for (size_t i = 0; i < len; i++)
		p[i] = a[i];

	return FeString(p, len);
}

FeString operator "" _fe(const wchar_t* a, size_t len) {
	wchar_t* p = Memory::get()->allocType<wchar_t>(len + 1ULL);
	Memory::get()->copyType<wchar_t>(p, a, len);
	return FeString(p, len);
}
#pragma endregion



