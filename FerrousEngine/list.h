#pragma once
#include "collection.h"
#include "allocation.h"

namespace fe {
	namespace collections {
		template<typename T>
		class list : public Collection<T> {

		public:
			list(int initialCapacity, FerrousAllocator* allocator = Memory::get()) {

			}
l
			virtual BaseIterator<T> begin() override {
				return iterator(this, 0);
			}

			virtual BaseIterator<T> end() override {
				return iterator(this, _count);
			}

			virtual void clear() override {

			}

			virtual bool isEmpty() override {

			}

			class iterator : public BaseIterator {

			};
		};
	}
}