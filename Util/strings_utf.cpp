#include "strings.h"
#include "memory.h"

/* Lookup data table for UTF byte order marks (BOMs)*/
const char UtfString::UTF_BOM_8_NON[] = { 0 };
const char UtfString::UTF_BOM_8[] = { 3, 0xEF, 0xBB, 0xBF };
const char UtfString::UTF_BOM_16_BE[] = { 2, 0xFE, 0xFF };
const char UtfString::UTF_BOM_16_LE[] = { 2, 0xFF, 0xFE };
const char* UtfString::UTF_BOM[] = { UtfString::UTF_BOM_8_NON, UtfString::UTF_BOM_8_NON, UtfString::UTF_BOM_8, UtfString::UTF_BOM_16_BE, UtfString::UTF_BOM_16_LE };

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

UtfString::UtfString() : _isHeap(false) {
	_length = 0;
	_allocator = Memory::get();
	_num_bytes = 0;
	_encoding = UtfEncoding::Auto;
	_mem = nullptr;
	_data = _mem;
}

UtfString::UtfString(const FeString& string, UtfEncoding encoding, FerrousAllocator* allocator, bool isHeap) : _isHeap(isHeap) {
	_length = string.len();
	_allocator = allocator;
	_num_bytes = 0;
	_encoding = encoding;
	size_t required_bytes = (sizeof(char32_t) * _length);
	_mem = static_cast<char*>(_allocator->alloc(required_bytes)); // Alloc for worst case scenario of 4-bytes per char.
	_data = _mem;

	switch (encoding) {
	case UtfEncoding::Auto:
	case UtfEncoding::UTF8: // https://en.wikipedia.org/wiki/UTF-8
	case UtfEncoding::UTF8_WithBOM:
		encode_utf8(string, required_bytes);
		break;

	case UtfEncoding::UTF16_LE: encode_utf16_le(string, required_bytes); break;
	case UtfEncoding::UTF16_BE: encode_utf16_be(string, required_bytes); break;

	default:
		// TODO throw exception for invalid encoding
		break;
	}
}

UtfString::UtfString(const FeString& string, UtfEncoding encoding, bool isHeap) : UtfString(string, encoding, string.getAllocator(), isHeap) {}

UtfString::UtfString(char* utfData, size_t numBytes, UtfEncoding encoding, FerrousAllocator* allocator, bool isHeap) : _isHeap(isHeap) {
	_encoding = encoding;
	_num_bytes = numBytes;
	_allocator = allocator;
	_mem = utfData;
	_data = _mem;
	_length = getNumChars(utfData, numBytes, encoding);
}

uint32_t UtfString::getNumChars(const char* data, size_t numBytes, UtfEncoding encoding) {
	uint32_t len = 0;

	switch (encoding) {
	case UtfEncoding::Auto:
	case UtfEncoding::UTF8: // https://en.wikipedia.org/wiki/UTF-8
	case UtfEncoding::UTF8_WithBOM:
		for (size_t i = 0; i < numBytes; i++) {
			if (!(data[i] & 128) || ((data[i] & 192) == 192)) {// ASCII or UTF8-lead bit
				len++;
			}
		}
		break;

	case UtfEncoding::UTF16_LE:
		throw "Not implemented";
		break;
	case UtfEncoding::UTF16_BE:
		throw "Not implemented";
		break;

	default:
		// TODO throw exception for invalid encoding
		break;
	}

	return len;
}

UtfString::UtfString(const UtfString& copy) : _isHeap(copy._isHeap) {
	_allocator = copy._allocator;
	_encoding = copy._encoding;
	_mem = copy._mem;
	_data = copy._data;
	_allocator->ref(_mem);
	_length = copy._length;
	_num_bytes = copy._num_bytes;
}

UtfString::~UtfString() {
	if(_isHeap)
		_allocator->deref(_mem);
}

UtfString& UtfString::operator = (const UtfString& other) {
	return *this;
}

void UtfString::encode_utf8(const FeString& string, size_t max_bytes) {
	_data += max_bytes - 1;

	char* r_end = _data;
	const wchar_t* src = string.getData();
	uint32_t pos = string.len() + 1; // Start with the null terminator

	while (pos > 0) {
		pos--;

		if (src[pos] < 128) {
			*_data = (char)src[pos];
		}
		else {
			uint8_t char_bytes = 1;
			wchar_t c = src[pos];
			while (c > UTF8_LEAD_CAPACITY[char_bytes]) {
				*_data = UTF8_TRAIL_MASK | (c & 63); // first 6 bits = c & (32 | 16 | 8 | 4 | 2 | 1)
				c = c >> 6;
				char_bytes++;
				_data--;
			}

			*_data = (UTF8_LEAD_MASK[char_bytes]) | c;
		}

		_data--;
	}

	_data++;
	_num_bytes = r_end - _data;
}

void UtfString::encode_utf16_le(const FeString& string, size_t max_bytes) {
	char16_t* utfData = reinterpret_cast<char16_t*>(_data);

	const wchar_t* src = string.getData();
	uint32_t pos = 0;

	// TODO separate each encoding into its own function. UTF-16 does not need to work backwards.
	while (pos < _length) {
		wchar_t c = src[pos];
		if ((c >= 0x0000 && c <= 0xD7FF) ||		// Reserved codepoint?
			(c >= 0xE000 && c <= 0xFFFF)) {		// Surrogate pair?
			*utfData = c;
		}
		else if (c >= 0xD800 && c <= 0xDFFF) {		// Reserved codepoint
			*utfData = 0xFFFD; // Unicode replacement character
		}
		else {	// Surrogate pair
			c -= 0x10000;
			*utfData = ((c >> 10) / 0x400) + 0xD800; // high byte
			utfData++;
			*utfData = (c & 1023) + 0xDC00; // low byte
		}

		pos++;
		utfData++;
	}

	_num_bytes = (utfData - reinterpret_cast<char16_t*>(_data)) * sizeof(char16_t);
}

void UtfString::encode_utf16_be(const FeString& string, size_t max_bytes) {
	throw "Not implemented";
}
FeString UtfString::decode(const char* data, UtfEncoding dataEncoding, size_t numBytes, FerrousAllocator* allocator) {
	uint32_t len = getNumChars(data, numBytes, dataEncoding);
	return decode(data, len, dataEncoding, allocator);
}

FeString UtfString::decode(const char* data, uint32_t numChars, UtfEncoding data_encoding, FerrousAllocator* allocator) {
	wchar_t* mem = allocator->allocType<wchar_t>(numChars + 1U); // 1 extra for the null-terminator.
	size_t pos = 0;
	const char* temp = data;

	switch (data_encoding) {
	case UtfEncoding::Auto:
	case UtfEncoding::UTF8: // https://en.wikipedia.org/wiki/UTF-8
	case UtfEncoding::UTF8_WithBOM:
		while (pos <= numChars) {
			if (!(*temp & 128)) { // Is the last bit set? If false, it's ASCII
				mem[pos] = *temp;
				temp++;
			}
			else {
				uint8_t b = *temp;
				char32_t c = 0;
				uint8_t num_bytes = 0;
				bool first_byte = true; // First byte tells us how many UTF-8 bytes a character is made up of.

				while (b) {
					uint8_t bit = 128;

					while (bit) {
						if (first_byte) {
							if (b & bit) {
								num_bytes++;
								bit >>= 1;
							}
							else {
								first_byte = false;
								bit >>= 1;
								break;
							}
						}
						else {
							// All bytes after the first byte should have a bit pattern of 10xxxxxx
							if ((b & 192) == 128)
								bit >>= 2;

							break;
						}
					}

					// Now read char bit data from the remainder of the current byte.
					while (bit) {
						c = (c << 1) | ((b & bit) ? 1 : 0);
						bit >>= 1;
					}

					temp++;
					b = *temp;
					num_bytes--;
					if (num_bytes == 0)
						break;
				}

				mem[pos] = c > WCHAR_MAX ? L'?' : c;
			}
			pos++;
		}

		mem[--pos] = L'\0';
		break;

	case UtfEncoding::UTF16_LE:

		break;

	case UtfEncoding::UTF16_BE:

		break;
	}

	return FeString(mem, numChars, allocator);
}