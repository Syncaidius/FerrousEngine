#pragma once
#include <exception>
#include <stdint.h>

namespace fe {
	struct IndexOutOfRangeExeption : public std::exception {
		inline IndexOutOfRangeExeption(uint32_t index, uint32_t maxIndex) :
			_msg("Index out of range."), _index(index), _maxIndex(maxIndex) {}

		inline const char* message() const throw() {
			return _msg;
		}

		inline const uint32_t maxIndex() const {
			return _maxIndex;
		}

		inline const uint32_t index() const {
			return _index;
		}

	private:
		const char* _msg;
		const uint32_t _maxIndex;
		const uint32_t _index;
	};
}