#pragma once
#include "stdafx.h"
#include "allocation.h"
#include <iostream>
#include <cstdarg>

namespace fe {
	enum struct UtfEncoding : uint8_t {
		/* Auto-detect or select the encoding, where possible. */
		Auto = 0,

		/* UTF-8, no BOM */
		UTF8 = 1,

		/* UTF-8, with BOM */
		UTF8_WithBOM = 2,

		/* UTF-16 Big Endian (BE) */
		UTF16_BE = 3,

		/* UTF-16 Little Endian (LE).*/
		UTF16_LE = 4,
	};

	class FeString {
	public:
		class FormatArg {
			FormatArg(int value);
			FormatArg(double value);
		};

		/* The value returned by indexOf() if no instance was found. */
		const static uint32_t INDEXOF_NONE;

		/* An empty string. */
		const static FeString EMPTY;

		template<class... FormatArg>
		static FeString format(const FeString& str, FerrousAllocator* allocator, FormatArg&& ... args) {
			/*static size_t buf_size = 80;
			static thread_local char32_t* buffer = Memory::get()->allocType<char32_t>(buf_size); // Use main allocator for the format buffer.
			int len_v = vswprintf(buffer, buf_size, str, args);

			/* Buffer too small? */
			/*while (len_v < 0) {
				buf_size *= 2;
				Memory::get()->reallocType<char32_t>(buffer, buf_size);

				// Try again. Should work this time.
				len_v = vswprintf(buffer, buf_size, str, args);
			}

			char32_t* mem = allocator->allocType<char32_t>(len_v + 1ULL);
			memcpy(mem, buffer, len_v * sizeof(char32_t));
			mem[len_v] = U'\0';
			return FeString(mem, len_v, allocator, true);*/
			return FeString(str);
		}

		template<class... FormatArg>
		static FeString format(const FeString& str, FormatArg&& ... args) {
			return format(str._data, str._allocator, std::forward<FormatArg>(args)...);
		}

		static FeString repeat(const FeString& str, uint32_t count, FerrousAllocator* allocator = Memory::get());

		/* Returns a new FeString containing the current date and/or time. Uses wcsftime formatting rules. See: https://en.cppreference.com/w/cpp/chrono/c/wcsftime */
		static FeString dateTime(const FeString& format, FerrousAllocator* allocator = Memory::get());

		FeString();
		FeString(const char* data, uint32_t len, FerrousAllocator* allocator = Memory::get());
		FeString(const char* data, FerrousAllocator* allocator = Memory::get());

		FeString(const wchar_t* data, uint32_t len, FerrousAllocator* allocator = Memory::get());
		FeString(const wchar_t* data, FerrousAllocator* allocator = Memory::get());

		FeString(const char32_t* data, uint32_t len, FerrousAllocator* allocator = Memory::get(), bool isHeap = true);
		FeString(const char32_t* data, FerrousAllocator* allocator);

		/* Treats the provided wide-char data as non-heap data. This means it will not be dereferenced once the string is deconstructed.*/
		FeString(const char32_t* data);

		FeString(const FeString& copy);

		~FeString();

		FeString substr(const uint32_t startIndex);
		FeString substr(const uint32_t startIndex, const uint32_t count);

		FeString replace(const char32_t c, const char32_t replacement);
		FeString replace(const FeString str, const FeString replacement);

		bool startsWith(const char32_t c);
		bool startsWith(const FeString* input);

		bool endsWith(const char32_t input);
		bool endsWith(const FeString* input);

		bool contains(const char32_t c);
		bool contains(const FeString* input);

		/* Returns the index of the first instance of c. Returns SIZE_MAX if none were found.*/
		uint32_t indexOf(const char32_t c);

		/* Returns the index of the first instance of input. Returns SIZE_MAX if none were found.*/
		uint32_t indexOf(const FeString* input);

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

		void toChar8(char* buffer) const;

		void toChar16(char16_t* buffer) const;

		inline FerrousAllocator* getAllocator() const { return _allocator; }

		/* Gets the length of the string.*/
		const inline uint32_t len() const { return _length; }

		/*Returns a pointer to the raw underlying character data.*/
		const inline char32_t* getData() const { return _data; }

		/* Copy assignment operator*/
		FeString& operator=(const FeString& other);

		bool operator==(const FeString& other) const {
			if (_length != other._length)
				return false;

			for (uint32_t i = 0; i < _length; i++) {
				if (_data[i] != other._data[i])
					return false;
			}

			return true;
		}

	private:
		friend FeString operator +(const FeString& a, const FeString& b);
		friend FeString operator +(const FeString& a, const uint8_t& v);
		friend FeString operator +(const FeString& a, const uint16_t& v);
		friend FeString operator +(const FeString& a, const uint32_t& v);
		friend FeString operator +(const FeString& a, const uint64_t& v);
		friend FeString operator +(const FeString& a, const int8_t& v);
		friend FeString operator +(const FeString& a, const int16_t& v);
		friend FeString operator +(const FeString& a, const int32_t& v);
		friend FeString operator +(const FeString& a, const int64_t& v);
		friend FeString operator +(const FeString& a, const double& v);
		friend FeString operator +(const FeString& a, const long double& v);
		friend FeString operator +(const FeString& a, const float& v);

		friend FeString operator "" _fe(const char* c_data, size_t len);
		friend FeString operator "" _fe(const wchar_t* c_data, size_t len);
		friend FeString operator "" _fe(const char32_t* a, size_t len);

		const char32_t* _data;			/* The raw, unencoded character data. */
		FerrousAllocator* _allocator;	/* The allocator from which _data was allocated. */
		uint32_t _length;			/* Number of characters held in the string, excluding the null-terminator. */
		const bool _isHeap;			/* String was created from a literal and therefore should never dealloc its data. */
	};

	class UtfString {
	public:
		const static char UTF_BOM_8_NON[];
		const static char UTF_BOM_8[];
		const static char UTF_BOM_16_BE[];
		const static char UTF_BOM_16_LE[];
		const static char* UTF_BOM[];
		static constexpr size_t UTF_BOM_COUNT = 5;

		UtfString();
		UtfString(const UtfString& copy);

		/* Creates a new UtfString by re-encoding an FeString string to the specified encoding. */
		UtfString(const FeString& string, UtfEncoding encoding, FerrousAllocator* allocator, bool isHeap = true);
		UtfString(char* utfData, size_t num_bytes, UtfEncoding encoding, FerrousAllocator* allocator, bool isHeap = true);
		UtfString(const FeString& string, UtfEncoding encoding, bool isHeap = true);
		~UtfString();

		UtfString& operator = (const UtfString& other);

		/* Decodes a UTF byte array into an FeString and returns it.*/
		static FeString decode(const char* data, uint32_t numChars, UtfEncoding dataEncoding, FerrousAllocator* allocator);

		/* Decodes a UTF byte array with an unknown number of characters. The character length will be calculated during decoding. */
		static FeString decode(const char* data, UtfEncoding dataEncoding, size_t numBytes, FerrousAllocator* allocator);

		/* Decodes the current UtfString into an FeString and returns it.*/
		inline FeString decode(FerrousAllocator* allocator) const {
			return decode(_data, _length, _encoding, allocator);
		}

		/* Decodes the current UtfString into an FeString and returns it.*/
		inline FeString decode() const {
			return decode(_data, _length, _encoding, _allocator);
		}

		inline const size_t byteLen() const { return _num_bytes; }
		inline const size_t len() const { return _length; }
		inline const char* getData() const { return _data; }
		inline const UtfEncoding getEncoding() const { return _encoding; }

	private:
		const static uint8_t UTF8_LEAD_MASK[];
		const static uint8_t UTF8_LEAD_CAPACITY[];
		const static uint8_t UTF8_TRAIL_MASK;

		void encode_utf8(const FeString& src, size_t max_bytes);
		void encode_utf16_le(const FeString& src, size_t max_bytes);
		void encode_utf16_be(const FeString& src, size_t max_bytes);
		static uint32_t getNumChars(const char* data, size_t numBytes, UtfEncoding encoding);

		char* _data;
		char* _mem;
		FerrousAllocator* _allocator;
		size_t _num_bytes;
		size_t _length;
		UtfEncoding _encoding;
		const bool _isHeap;
	};

#pragma region OPERATORS
	FeString operator +(const FeString& a, const FeString& b);
	//FeString operator +(const FeString& a, const uint8_t& v);
	//FeString operator +(const FeString& a, const uint16_t& v);
	//FeString operator +(const FeString& a, const uint32_t& v);
	//FeString operator +(const FeString& a, const uint64_t& v);
	//FeString operator +(const FeString& a, const int8_t& v);
	//FeString operator +(const FeString& a, const int16_t& v);
	//FeString operator +(const FeString& a, const int32_t& v);
	//FeString operator +(const FeString& a, const int64_t& v);
	//FeString operator +(const FeString& a, const double& v);
	//FeString operator +(const FeString& a, const long double& v);
	//FeString operator +(const FeString& a, const float& v);

	FeString operator "" _fe(const char* a, size_t len);
	FeString operator "" _fe(const wchar_t* a, size_t len);
	FeString operator "" _fe(const char32_t* a, size_t len);


#pragma endregion
}