#include "strings.h"
#include "memory.h"
#include "localization.h"
#include <vector>

#pragma region STATIC
const uint32_t FeString::INDEXOF_NONE = UINT32_MAX;
const FeString FeString::EMPTY(L"");

FeString FeString::format(const wchar_t* str, FerrousAllocator* allocator, va_list args) {
	static size_t buf_size = 80;
	static wchar_t* buf = Memory::get()->allocType<wchar_t>(buf_size); // Use main allocator for the format buffer.
	int len_v = vswprintf(buf, buf_size, str, args);

	/* Buffer too small? */
	while (len_v < 0) {
		size_t old_size = buf_size;
		buf_size = (buf_size * 2) + len_v; // Double size and ensure it will be enough for the current value too.
		Memory::get()->reallocType<wchar_t>(buf, old_size, buf_size);

		// Try again. Should work this time.
		len_v = vswprintf(buf, buf_size, str, args);
	}

	wchar_t* mem = allocator->allocType<wchar_t>(len_v + 1ULL);
	memcpy(mem, buf, len_v * sizeof(wchar_t));
	mem[len_v] = L'\0';
	return FeString(mem, len_v, allocator, false);
}

FeString FeString::format(const FeString& str, FerrousAllocator* allocator, va_list args) {
	return format(str.getData(), allocator, args);
}

FeString FeString::format(const wchar_t* str, FerrousAllocator* allocator, ...) {
	va_list args;
	va_start(args, str);
	FeString result = format(str, allocator, args);
	va_end(args);
	return result;
}

FeString FeString::format(const FeString& str, FerrousAllocator* allocator, ...) {
	va_list args;
	va_start(args, str);
	FeString result = format(str.getData(), allocator, args);
	va_end(args);
	return result;
}

FeString FeString::format(const FeString& str, ...) {
	va_list args;
	va_start(args, str);
	FeString result = format(str, Memory::get(), args);
	va_end(args);
	return result;
}

FeString FeString::format(const FeString& str, va_list args) {
	return format(str, Memory::get(), args);
}

FeString FeString::dateTime(const wchar_t* format, FerrousAllocator* allocator) {
	static size_t buf_size = 80;
	static wchar_t* buf = Memory::get()->allocType<wchar_t>(buf_size);

	assert(format != nullptr);
	time_t now = time(nullptr);
	struct tm tstruct;
	localtime_s(&tstruct, &now);
	size_t len = wcsftime(buf, buf_size, format, &tstruct);
	if (len == 0) { // buffer was not big enough
		size_t old_size = buf_size;
		buf_size *= 2;

		Memory::get()->reallocType<wchar_t>(buf, old_size, buf_size);
		return dateTime(format, allocator);
	}

	wchar_t* mem = allocator->allocType<wchar_t>(len + 1ULL);
	memcpy(mem, buf, len * sizeof(wchar_t));
	mem[len] = L'\0';
	return FeString(mem, len, allocator, false);
}
#pragma endregion

#pragma region INSTANCED
FeString::FeString() : _isHeap(false) {
	_allocator = Memory::get();
	_data = FeString::EMPTY.getData();
	_length = 0;
}

FeString::FeString(const FeString& copy) : _isHeap(copy._isHeap) {
	_allocator = copy._allocator;
	_data = copy._data;
	_length = copy._length;

	if(!_isHeap)
		_allocator->ref(_data);
}

FeString::FeString(const char* data, uint32_t len, FerrousAllocator* allocator) : _isHeap(true) {
	_length = len;
	_allocator = allocator;

	wchar_t* mem = allocator->allocType<wchar_t>(len + 1ULL);
	for (size_t i = 0; i < _length; i++)
		mem[i] = data[i];

	mem[_length] = L'\0';
	_data = mem;
}

FeString::FeString(const wchar_t* data, uint32_t len, FerrousAllocator* allocator, bool isHeap) : _isHeap(isHeap) {
	_length = len;
	_allocator = allocator;
	_data = data;
	if (_data[_length] != L'\0')
		throw std::exception("wide-character string is not null-terminated.");
}

FeString::FeString(const char32_t* c_data, uint32_t len, FerrousAllocator * allocator) : _isHeap(true) {
	_length = len;
	_allocator = allocator;

	wchar_t* mem = allocator->allocType<wchar_t>(len + 1ULL);
	for (uint32_t i = 0; i < len; i++)
		mem[i] = c_data[i] > WCHAR_MAX ? '?' : c_data[i];

	mem[len] = L'\0';
	_data = mem;
}

FeString::FeString(const char32_t* c_data, FerrousAllocator * allocator) : _isHeap(true) {
	_allocator = allocator;
	while (c_data[_length] != U'\0')
		_length++;

	wchar_t* mem = allocator->allocType<wchar_t>(_length + 1ULL);
	for (uint32_t i = 0; i < _length; i++)
		mem[i] = c_data[i] > WCHAR_MAX ? '?' : c_data[i];

	mem[_length] = L'\0';
	_data = mem;

}

FeString::FeString(const char* data, uint32_t len) : FeString(data, len, Memory::get()) {}
FeString::FeString(const char* data, FerrousAllocator* allocator) : FeString(data, strlen(data), allocator) {}
FeString::FeString(const char* data) : FeString(data, strlen(data), Memory::get()) {}

FeString::FeString(const wchar_t* data, uint32_t len) : FeString(data, len, Memory::get()) {}
FeString::FeString(const wchar_t* data, FerrousAllocator* allocator) : FeString(data, wcslen(data), allocator) {}
FeString::FeString(const wchar_t* data) : FeString(data, wcslen(data), Memory::get()) {}

FeString::FeString(const char32_t* data, uint32_t len) : FeString(data, len, Memory::get()) {}
FeString::FeString(const char32_t* data) : FeString(data, Memory::get()) {}

FeString::~FeString() {
	if(!_isHeap)
		_allocator->deref((void*)_data);

	_allocator = nullptr;
}

FeString FeString::toLower() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = _allocator->allocType<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();

	for (uint32_t i = 0; i < _length; i++)
		new_data[i] = std::tolower(_data[i], loc);

	new_data[_length] = L'\0';
	return FeString(new_data, _length, _allocator);
}

FeString FeString::toUpper() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = _allocator->allocType<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();

	for (uint32_t i = 0; i < _length; i++)
		new_data[i] = std::toupper(_data[i], loc);

	new_data[_length] = L'\0';
	return FeString(new_data, _length, _allocator);
}

FeString FeString::capitalize() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = _allocator->allocType<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();
	bool capitalize = true;

	for (uint32_t i = 0; i < _length; i++) {
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
	return FeString(new_data, _length, _allocator);
}

FeString FeString::capitalizeFirst() {
	if (_length == 0)
		return FeString();

	wchar_t* new_data = _allocator->allocType<wchar_t>(_length + 1ULL);
	std::locale loc = *Localization::get()->getCurrentLocale();
	bool capitalize = true;

	/* Loop until we hit a non-whitespace character*/
	for (uint32_t i = 0; i < _length; i++) {
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
	return FeString(new_data, _length, _allocator);
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

	wchar_t* new_data = _allocator->allocType<wchar_t>(new_len + 1ULL);
	const wchar_t* p_start = _data + start;
	memcpy(new_data, p_start, new_len * sizeof(wchar_t));

	new_data[new_len] = L'\0';
	return FeString(new_data, new_len, _allocator);
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

	wchar_t* new_data = _allocator->allocType<wchar_t>(new_len + 1ULL);
	const wchar_t* p_start = _data + start;
	memcpy(new_data, p_start, new_len * sizeof(wchar_t));

	new_data[new_len] = L'\0';
	return FeString(new_data, new_len, _allocator);
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

	wchar_t* new_data = _allocator->allocType<wchar_t>(new_len + 1ULL);
	memcpy(new_data, _data, new_len * sizeof(wchar_t));

	new_data[new_len] = L'\0';
	return FeString(new_data, new_len, _allocator);
}

uint32_t FeString::indexOf(const wchar_t c) {
	for (uint32_t i = 0; i < _length; i++) {
		if (_data[i] == c)
			return i;
	}

	return INDEXOF_NONE;
}

uint32_t FeString::indexOf(const FeString* input) {
	assert(input != nullptr);

	if (_length >= input->_length)
	{
		for (uint32_t i = 0; i < _length; i++) {
			if (_data[i] == input->_data[0]) {
				/* Already checked first char. */
				uint32_t j = 1;

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

FeString FeString::substr(const uint32_t startIndex) {
	assert(startIndex < _length);

	uint32_t count = _length - startIndex;
	wchar_t* mem = _allocator->allocType<wchar_t>(count + 1ULL);
	const wchar_t* src_pos = _data + startIndex;
	memcpy(mem, src_pos, count * sizeof(wchar_t));

	mem[count] = L'\0';
	return FeString(mem, count, _allocator);
}

FeString FeString::substr(const uint32_t startIndex, const uint32_t count) {
	assert((startIndex + count) < _length);

	wchar_t* mem = _allocator->allocType<wchar_t>(count + 1ULL);
	const wchar_t* src_pos = _data + startIndex;
	Memory::copy(mem, src_pos, count * sizeof(wchar_t));

	mem[count] = L'\0';
	return FeString(mem, count, _allocator);
}

FeString FeString::replace(const wchar_t c, const wchar_t replacement) {
	wchar_t* mem = _allocator->allocType<wchar_t>(_length + 1ULL);
	memcpy(mem, _data, (_length + 1ULL) * sizeof(wchar_t));

	for (uint32_t i = 0; i < _length; i++) {
		if (mem[i] == c)
			mem[i] = replacement;
	}

	mem[_length] = L'\0';
	return FeString(mem, _length, _allocator);
}

FeString FeString::replace(const FeString input, const FeString replacement) {
	/* If the input string is large than the current/target, it will never match. Early return. */
	if (_length < input._length)
		return FeString(*this);

	std::vector<size_t> indices; // TODO: Check if this allocates cleanly. i.e. not dynamically allocating/deallocating via malloc/free.

	// Collect index of every match.
	for (size_t i = 0; i < _length; i++) {
		if (_data[i] == input._data[0]) {
			/* Already checked first char. */
			size_t j = 1;

			/* Now iterate over _data from i to see if the rest of the string matches. */
			for (; j < input._length; j++) {
				if ((i + j >= _length) || (_data[i + j] != input._data[j]))
					break;
			}

			if (j == input._length)
				indices.push_back(i);
		}
	}

	// Build new string
	size_t replacement_bytes = replacement._length * sizeof(wchar_t);
	size_t count = indices.size();
	size_t new_len = _length;

	// TODO find a better way to handle negative input_dif, instead of branching.
	if (replacement._length >= input._length) {
		uint32_t input_dif = replacement._length - input._length;
		new_len = _length + (input_dif * count);
	}
	else { // Input is smaller than the replacement
		uint32_t input_dif = input._length - replacement._length;
		new_len = _length - (input_dif * count);
	}

	uint32_t prev_replace_end = 0; /* Ending of previous replacement.*/

	wchar_t* mem = _allocator->allocType<wchar_t>(new_len + 1ULL);
	wchar_t* mem_pos = mem;

	for (size_t i = 0; i < count; i++) {
		/*	Was there non-replaced data between the current and previous replacement?
			If so, copy it. */
		if (indices[i] != prev_replace_end) {
			uint32_t dif = indices[i] - prev_replace_end;
			memcpy(mem_pos, _data + prev_replace_end, dif * sizeof(wchar_t));
			mem_pos += dif;
		}

		/* Now copy the replacement string into new string.*/
		memcpy(mem_pos, replacement._data, replacement_bytes);
		mem_pos += replacement._length;
		prev_replace_end = indices[i] + input._length;
	}

	assert(prev_replace_end <= _length);
	if (prev_replace_end < _length) {
		uint32_t dif = _length - prev_replace_end;
		memcpy(mem_pos, _data + prev_replace_end, dif * sizeof(wchar_t));
	}

	mem[new_len] = L'\0';
	return FeString(mem, new_len, _allocator);
}

bool FeString::contains(const wchar_t c) {
	for (uint32_t i = 0; i < _length; i++) {
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
		uint32_t start = _length - input->_length;

		for (uint32_t i = start; i < _length; i++) {
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
		uint32_t i = 0;

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
FeString& FeString::operator=(const FeString& other) {
	if (this != &other) {
		_length = other._length;
		_data = other._data;
		_allocator = other._allocator;
		_allocator->ref(_data);
	}
	return *this;
}

FeString operator +(const FeString& a, const FeString& b) {
	uint32_t len = a._length + b._length;

	wchar_t* mem = a._allocator->allocType<wchar_t>(len + 1ULL);
	wchar_t* p_data = mem;

	memcpy(p_data, a._data, a._length * sizeof(wchar_t));
	p_data += a._length;
	memcpy(p_data, b._data, b._length * sizeof(wchar_t));
	mem[len] = '\0';

	return FeString(mem, len, a._allocator);
}

FeString operator +(const FeString& a, const uint8_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const uint16_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const uint32_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const uint64_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const int8_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const int16_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const int32_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const int64_t& v) { return FeString::format(L"%d", a._allocator, v); }
FeString operator +(const FeString& a, const double& v) { return FeString::format(L"%f", a._allocator, v); }
FeString operator +(const FeString& a, const long double& v) { return FeString::format(L"%f", a._allocator, v); }
FeString operator +(const FeString& a, const float& v) { return FeString::format(L"%f", a._allocator, v); }

FeString operator "" _fe(const char* a, size_t len) {
	return FeString(a, len, Memory::get());
}

FeString operator "" _fe(const wchar_t* a, size_t len) {
	return FeString(a, len, Memory::get(), true);
}

FeString operator "" _fe(const char32_t* a, size_t len) {
	return FeString(a, len, Memory::get());
}
#pragma endregion



