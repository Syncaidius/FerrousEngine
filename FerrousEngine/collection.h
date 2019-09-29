#pragma once
#include "stdafx.h"

namespace fe {
	namespace collections {
		// Base class for all ferrous interator types.
		template<typename T>
		class BaseIterator {
		public:
			// The 5 std::iterator properties, since they're no longer provided by std::iterator in C++ 17.
			// These describe how and what the iterator provides.
			using value_type = T;
			using reference = T&;
			using iterator_category = std::input_iterator_tag; // See: https://en.cppreference.com/w/cpp/iterator/iterator_tags
			using pointer = T*;
			using difference_type = void;
		};

		// Base class for all ferrous collection types.
		template<typename T>
		class Collection {
		public:
			virtual BaseIterator<T> begin() = 0;
			virtual BaseIterator<T> end() = 0;

			virtual void clear() = 0;
			virtual bool isEmpty() = 0;

			inline uint32_t count() const { return _count; }

		protected:
			uint32_t _count;
		};
	}
}