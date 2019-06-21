#include "strings.h"
#include "memory.h"

/* Lookup data table for UTF byte order marks (BOMs)*/
const char UtfString::UTF_BOM_8_NON[] = { 0 };
const char UtfString::UTF_BOM_8[] = { 3, 0xEF, 0xBB, 0xBF };
const char UtfString::UTF_BOM_16_BE[] = { 2, 0xFE, 0xFF };
const char UtfString::UTF_BOM_16_LE[] = { 2, 0xFF, 0xFE };
const char* UtfString::UTF_BOM[] = { UtfString::UTF_BOM_8_NON, UtfString::UTF_BOM_8, UtfString::UTF_BOM_16_BE, UtfString::UTF_BOM_16_LE };

const uint8_t UtfString::UTF8_TRAIL_MASK = 0b10000000;
const uint8_t UtfString::UTF8_LEAD_MASK[] = {
	0b0, // Dummy for 0-bytes         - 0xxxxxxx
	0b0, // Dummy for 1-byte          - 0xxxxxxx
	0b11000000, // 2-byte char        - 110xxxxx
	0b11100000, // 3-byte char        - 1110xxxx
	0b11110000, // 4-byte char	      - 11110xxx
};

const uint8_t UtfString::UTF8_LEAD_CAPACITY[] = {
	0b01111111, // Dummy for 0-bytes  - 0xxxxxxx
	0b01111111, // Dummy for 1-byte   - 0xxxxxxx
	0b00011111, // 2-byte char        - 110xxxxx
	0b00001111, // 3-byte char        - 1110xxxx
	0b00000111, // 4-byte char        - 11110xxx
};

UtfString::UtfString(uint32_t len, UtfEncoding encoding, FerrousAllocator* allocator) {
	_length = len;
	_allocator = allocator;
	_num_bytes = 0;
	_encoding = encoding;

	switch (encoding) {
	case UtfEncoding::UTF8:
	case UtfEncoding::UTF8_WithBOM:
		_mem = static_cast<char*>(allocator->alloc(((size_t)len + 1) * 4)); // Alloc for worst case scenario of 4-bytes per char.
		_data = (_mem + _num_bytes) - 1;
		break;

	case UtfEncoding::UTF16_LE:

		break;

	case UtfEncoding::UTF16_BE:

		break;

	default:
		// TODO throw exception for invalid encoding
		break;
	}
}

UtfString::UtfString(const UtfString& copy) {
	_allocator = copy._allocator;
	_encoding = copy._encoding;
	_mem = copy._mem;
	_data = copy._data;
	_allocator->ref(_mem);
	_length = copy._length;
	_num_bytes = copy._num_bytes;

}

UtfString::~UtfString() {
	_allocator->deref(_mem);
}

UtfString UtfString::encode(const FeString* string, UtfEncoding encoding, FerrousAllocator* allocator) {
	UtfString result = UtfString(string->len(), encoding, allocator);
	size_t pos = string->len() + 1; // Start with the null terminator
	const wchar_t* data = string->c_str();

	char* r_end = result._data;

	switch (encoding) {
	case UtfEncoding::UTF8: // https://en.wikipedia.org/wiki/UTF-8
	case UtfEncoding::UTF8_WithBOM:
		while (pos > 0) {
			pos--;

			if (data[pos] < 128) {
				result._data[0] = (char)data[pos];
				result._data--;
			}
			else {
				uint8_t char_bytes = 1;
				wchar_t c = data[pos];
				while (c > UTF8_LEAD_CAPACITY[char_bytes]) {
					result._data[0] = UTF8_TRAIL_MASK | (c & 63); // first 6 bits = c & (32 | 16 | 8 | 4 | 2 | 1)
					c = c >> 6;
					char_bytes++;
					result._data--;
				}

				result._data[0] = (UTF8_LEAD_MASK[char_bytes]) | c;
				result._data--;
			}
		}
		result._data++;
		result._num_bytes = r_end - result._data;
		break;

	case UtfEncoding::UTF16_LE:

		break;

	case UtfEncoding::UTF16_BE:

		break;
	}

	return result;
}