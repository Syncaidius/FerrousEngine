#pragma once
#include "stdafx.h"
#include "iterator_forward_range.h"

namespace fe {
	namespace collections {
		// Base class for all ferrous collection types.
		template<typename T>
		class Collection {
		public:
			virtual void clear() = 0;

			virtual bool contains(T item) = 0;
			virtual bool add(T item) = 0;
			virtual bool remove(T item) = 0;

			virtual void copyTo(T* dest, uint32_t startIndex, uint32_t copyCount) = 0;

			inline uint32_t count() const { return _count; }
			inline bool isEmpty() const { return _count == 0; }

		protected:
			Collection() {
				_count = 0;
			}
			uint32_t _count;
		};
	}
}