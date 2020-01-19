#include "strings.h"
#include "localization.h"
#include <locale>
#include <vector>

namespace fe {
#pragma region STATIC
	const uint32_t FeString::INDEXOF_NONE = UINT32_MAX;
	const FeString FeString::EMPTY = U""_fe;

	FeString FeString::dateTime(const FeString& format, FerrousAllocator * allocator) {
		static thread_local size_t bufSize = 80;
		static thread_local size_t c8Size = 20;
		static thread_local char* timeBuf = Memory::get()->allocType<char>(bufSize);
		static thread_local char* c8FormatBuf = Memory::get()->allocType<char>(c8Size);

		// Check size of format buffer.
		if (c8Size <= format._length) {
			c8Size = (size_t)format._length + 1U;
			Memory::get()->reallocType(c8FormatBuf, c8Size);
		}

		format.toChar8(c8FormatBuf);

		time_t now = time(nullptr);
		struct tm tstruct;
		localtime_s(&tstruct, &now);

		size_t len = strftime(timeBuf, bufSize, c8FormatBuf, &tstruct);

		// Check size of time buffer.
		if (len == 0) {
			bufSize *= 2;
			Memory::get()->reallocType<char>(timeBuf, bufSize);
			return dateTime(format, allocator);
		}

		return FeString(timeBuf, len, allocator);
	}

	FeString FeString::repeat(const FeString& str, uint32_t count, FerrousAllocator* allocator) {
		uint32_t len = str._length * count;
		char32_t* mem = allocator->allocType<char32_t>((size_t)len + 1U);
		for (int i = 0; i < count; i++) {
			Memory::copyType<char32_t>(&mem[i * str._length], str.getData(), str._length);
		}

		mem[len] = U'\0';
		return FeString(mem, len, allocator);
	}
#pragma endregion

#pragma region INSTANCED
	FeString::FeString() : FeString(EMPTY) {}

	FeString::FeString(const FeString & copy) : _isHeap(copy._isHeap) {
		_allocator = copy._allocator;
		_data = copy._data;
		_length = copy._length;

		if (_isHeap)
			_allocator->ref(const_cast<char32_t*>(_data));
	}

	FeString::FeString(const char* data, uint32_t len, FerrousAllocator * allocator) : _isHeap(true) {
		_length = len;
		_allocator = allocator;

		char32_t* mem = allocator->allocType<char32_t>(len + 1ULL);
		for (size_t i = 0; i < _length; i++)
			mem[i] = data[i];

		mem[_length] = U'\0';
		_data = mem;
	}

	FeString::FeString(const char* data, FerrousAllocator* allocator) : FeString(data, strlen(data), allocator) {};

	FeString::FeString(const wchar_t* data, uint32_t len, FerrousAllocator* allocator) : _isHeap(true) {
		_allocator = allocator;
		_length = len;

		char32_t* mem = allocator->allocType<char32_t>(_length + 1ULL);
		for (size_t i = 0; i < _length; i++)
			mem[i] = data[i];

		mem[_length] = U'\0';
		_data = mem;
	}

	FeString::FeString(const wchar_t* data, FerrousAllocator* allocator) : _isHeap(true) {
		_allocator = allocator;

		_length = 0;
		while (data[_length] != L'\0') {
			_length++;
		}

		char32_t* mem = allocator->allocType<char32_t>(_length + 1ULL);
		for (size_t i = 0; i < _length; i++)
			mem[i] = data[i];

		mem[_length] = U'\0';
		_data = mem;
	}

	FeString::FeString(const char32_t* data, uint32_t len, FerrousAllocator* allocator, bool isHeap) : _isHeap(isHeap) {
		_length = len;
		_allocator = allocator;
		_data = data;
		if (_data[_length] != U'\0')
			throw std::exception("string data is not null-terminated.");
	}

	FeString::FeString(const char32_t* data) : _isHeap(false) {
		_allocator = Memory::get();

		_length = 0;
		while (data[_length] != U'\0') {
			_length++;
		}

		_data = data;
	}


	FeString::FeString(const char32_t* data, FerrousAllocator* allocator) : _isHeap(true) {
		_allocator = allocator;

		_length = 0;
		while (data[_length] != U'\0') {
			_length++;
		}

		char32_t* mem = allocator->allocType<char32_t>(_length + 1ULL);
		for (size_t i = 0; i < _length; i++)
			mem[i] = data[i];

		mem[_length] = U'\0';
		_data = mem;
	}

	FeString::~FeString() {
		if (_isHeap)
			_allocator->deref((void*)_data);

		_allocator = nullptr;
	}

	FeString FeString::toLower() {
		if (_length == 0)
			return FeString();

		char32_t* new_data = _allocator->allocType<char32_t>(_length + 1ULL);
		Culture* cul = Localization::get()->getCurrentCulture();

		for (uint32_t i = 0; i < _length; i++)
			new_data[i] = cul->toLower(_data[i]);

		new_data[_length] = U'\0';
		return FeString(new_data, _length, _allocator);
	}

	FeString FeString::toUpper() {
		if (_length == 0)
			return FeString();

		char32_t* new_data = _allocator->allocType<char32_t>(_length + 1ULL);
		Culture* cul = Localization::get()->getCurrentCulture();

		for (uint32_t i = 0; i < _length; i++)
			new_data[i] = cul->toUpper(_data[i]);

		new_data[_length] = U'\0';
		return FeString(new_data, _length, _allocator);
	}

	FeString FeString::capitalize() {
		if (_length == 0)
			return EMPTY;

		char32_t* new_data = _allocator->allocType<char32_t>(_length + 1ULL);
		Culture * cul = Localization::get()->getCurrentCulture();
		bool capitalize = true;

		for (uint32_t i = 0; i < _length; i++) {
			if (cul->isSpace(_data[i])) {
				capitalize = true;
				new_data[i] = _data[i];
			}
			else {
				if (capitalize) {
					new_data[i] = cul->toUpper(_data[i]);
					capitalize = false;
				}
				else {
					new_data[i] = _data[i];
				}
			}
		}

		new_data[_length] = U'\0';
		return FeString(new_data, _length, _allocator);
	}

	FeString FeString::capitalizeFirst() {
		if (_length == 0)
			return EMPTY;

		char32_t* new_data = _allocator->allocType<char32_t>(_length + 1ULL);
		Culture * cul = Localization::get()->getCurrentCulture();
		bool capitalize = true;

		/* Loop until we hit a non-whitespace character*/
		for (uint32_t i = 0; i < _length; i++) {
			if (cul->isSpace(_data[i])) {
				new_data[i] = _data[i];
			}
			else {
				if (capitalize) {
					new_data[i] = cul->toUpper(_data[i]);
					capitalize = false;
				}
				else {
					new_data[i] = _data[i];
				}
			}
		}

		new_data[_length] = U'\0';
		return FeString(new_data, _length, _allocator);
	}

	FeString FeString::trim() {
		if (_length == 0)
			return EMPTY;

		Culture* cul = Localization::get()->getCurrentCulture();
		size_t start = 0;
		size_t end = 0;

		// Trim from the start of the string
		for (start = 0; start < _length; start++) {
			if (!cul->isWhitespace(_data[start]))
				break;
		}

#pragma warning(push)
#pragma warning(disable: 6295) 

		// _length cannot be zero at this point, so will not wrap or overflow to create an infinite loop.
		for (end = _length - 1; end > start; end--) { // TODO fix end overflowing to uint32.max and causing an invalid-sized allocation!
			if (!cul->isWhitespace(_data[end])) {
				end++;
				break;
			}
		}
#pragma warning(pop)

		size_t new_len = end - start;
		if (new_len == 0)
			return EMPTY;

		char32_t* new_data = _allocator->allocType<char32_t>(new_len + 1ULL);
		const char32_t* p_start = _data + start;
		memcpy(new_data, p_start, new_len * sizeof(char32_t));

		new_data[new_len] = U'\0';
		return FeString(new_data, new_len, _allocator);
	}

	FeString FeString::trimStart() {
		if (_length == 0)
			return FeString();

		Culture* cul = Localization::get()->getCurrentCulture();
		size_t start = 0;

		for (start = 0; start < _length; start++) {
			if (!cul->isWhitespace(_data[start]))
				break;
		}

		size_t new_len = _length - start;
		if (new_len == 0)
			return EMPTY;

		char32_t* new_data = _allocator->allocType<char32_t>(new_len + 1ULL);
		const char32_t* p_start = _data + start;
		memcpy(new_data, p_start, new_len * sizeof(char32_t));

		new_data[new_len] = U'\0';
		return FeString(new_data, new_len, _allocator);
	}

	FeString FeString::trimEnd() {
		if (_length == 0)
			return EMPTY;

		Culture* cul = Localization::get()->getCurrentCulture();
		size_t new_len = 0;

#pragma warning(push)
#pragma warning(disable: 6295) // _length cannot be zero at this point, so will not wrap or overflow to create an infinite loop.
		for (new_len = _length - 1; new_len >= 0; new_len--) {
			if (!cul->isWhitespace(_data[new_len])) {
				new_len++;
				break;
			}
		}
#pragma warning(pop)
		if (new_len == 0)
			return EMPTY;

		char32_t* new_data = _allocator->allocType<char32_t>(new_len + 1ULL);
		memcpy(new_data, _data, new_len * sizeof(char32_t));

		new_data[new_len] = U'\0';
		return FeString(new_data, new_len, _allocator);
	}

	uint32_t FeString::indexOf(const char32_t c) {
		for (uint32_t i = 0; i < _length; i++) {
			if (_data[i] == c)
				return i;
		}

		return INDEXOF_NONE;
	}

	uint32_t FeString::indexOf(const FeString * input) {
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
		char32_t* mem = _allocator->allocType<char32_t>(count + 1ULL);
		const char32_t* src_pos = _data + startIndex;
		memcpy(mem, src_pos, count * sizeof(char32_t));

		mem[count] = U'\0';
		return FeString(mem, count, _allocator);
	}

	FeString FeString::substr(const uint32_t startIndex, const uint32_t count) {
		assert((startIndex + count) < _length);

		char32_t* mem = _allocator->allocType<char32_t>(count + 1ULL);
		const char32_t* src_pos = _data + startIndex;
		Memory::copy(mem, src_pos, count * sizeof(char32_t));

		mem[count] = U'\0';
		return FeString(mem, count, _allocator);
	}

	FeString FeString::replace(const char32_t c, const char32_t replacement) {
		char32_t* mem = _allocator->allocType<char32_t>(_length + 1ULL);
		Memory::copy(mem, _data, _length * sizeof(char32_t));

		for (uint32_t i = 0; i < _length; i++) {
			if (mem[i] == c)
				mem[i] = replacement;
		}

		mem[_length] = U'\0';
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
		size_t replacement_bytes = replacement._length * sizeof(char32_t);
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

		char32_t* mem = _allocator->allocType<char32_t>(new_len + 1ULL);
		char32_t* mem_pos = mem;

		for (size_t i = 0; i < count; i++) {
			/*	Was there non-replaced data between the current and previous replacement?
				If so, copy it. */
			if (indices[i] != prev_replace_end) {
				uint32_t dif = indices[i] - prev_replace_end;
				memcpy(mem_pos, _data + prev_replace_end, dif * sizeof(char32_t));
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
			memcpy(mem_pos, _data + prev_replace_end, dif * sizeof(char32_t));
		}

		mem[new_len] = U'\0';
		return FeString(mem, new_len, _allocator);
	}

	bool FeString::contains(const char32_t c) {
		for (uint32_t i = 0; i < _length; i++) {
			if (_data[i] == c)
				return true;
		}

		return false;
	}

	bool FeString::contains(const FeString * input) {
		return indexOf(input) != INDEXOF_NONE;
	}

	bool FeString::endsWith(const char32_t c) {
		return _length > 0 && _data[_length - 1] == c;
	}

	bool FeString::endsWith(const FeString * input) {
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

	bool FeString::startsWith(const char32_t c) {
		return _length > 0 && _data[0] == c;
	}

	bool FeString::startsWith(const FeString * input) {
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

	void FeString::toChar8(char* buffer) const {
		for (uint32_t i = 0; i < _length; i++) 
			buffer[i] = _data[i] < 128 ? _data[i] : '?';

		buffer[_length] = '\0';
	}

	void FeString::toChar16(char16_t* buffer) const {
		for (uint32_t i = 0; i < _length; i++)
			buffer[i] = _data[i] < 65535 ? _data[i] : '?';

		buffer[_length] = L'\0';
	}

#pragma endregion

#pragma region OPERATORS
	/* Copy assignment operator. */
	FeString& FeString::operator=(const FeString & other) {
		if (this != &other) {
			_length = other._length;

			if (this->_isHeap)
				_allocator->deref(const_cast<char32_t*>(_data));

			_data = other._data;
			_allocator = other._allocator;

			if (other._isHeap)
				_allocator->ref(const_cast<char32_t*>(_data));
		}
		return *this;
	}

	/* Addition/concatenate operator. */
	FeString operator +(const FeString & a, const FeString & b) {
		uint32_t len = a._length + b._length;

		char32_t* mem = a._allocator->allocType<char32_t>(len + 1ULL);
		memcpy(mem, a._data, a._length * sizeof(char32_t));
		memcpy(&mem[a._length], b._data, b._length * sizeof(char32_t));
		mem[len] = U'\0';
		return FeString(mem, len, a._allocator);
	}

	//FeString operator +(const FeString& a, const uint8_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const uint16_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const uint32_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const uint64_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const int8_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const int16_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const int32_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const int64_t& v) { return FeString::format(L"%d", a._allocator, v); }
	//FeString operator +(const FeString& a, const double& v) { return FeString::format(L"%f", a._allocator, v); }
	//FeString operator +(const FeString& a, const long double& v) { return FeString::format(L"%f", a._allocator, v); }
	FeString operator +(const FeString & a, const float& v) { return FeString::format(U"%f", a._allocator, v); }

	FeString operator "" _fe(const char* a, size_t len) {
		return FeString(a, len, Memory::get());
	}

	FeString operator "" _fe(const wchar_t* a, size_t len) {
		return FeString(a, len, Memory::get());
	}

	FeString operator "" _fe(const char32_t* a, size_t len) {
		return FeString(a, len, Memory::get(), false);
	}
#pragma endregion
}