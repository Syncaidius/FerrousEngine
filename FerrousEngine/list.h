#pragma once
#include "collection.h"
#include "allocation.h"
#include "exceptions.h"

namespace fe {
	namespace collections {
		template<typename T>
		class list : public Collection<T> {

		public:
			class iterator : public BaseIterator<T> {
				// TODO implement
			};

			inline list(int initialCapacity, FerrousAllocator* allocator = Memory::get()) {
				_allocator = allocator;
			}

			iterator begin() override {
				return iterator(this, 0);
			}

			iterator end() override {
				return iterator(this, _count);
			}

			inline bool add(T item) override {
				// TODO implement
			}

			inline bool addCollection(Collection<T> collection) {
				// TODO implement
			}

			inline bool remove(T item) override {
				for (int i = 0; i < _count; i++) {
					if (_first[i] == item) {
						removeAt(i);
						return true;
					}
				}

				return false;
			}

			inline void removeAt(uint32_t index) {
				if (index >= _count || index < 0)
					throw IndexOutOfRangeExeption(index, _count);

				// If not last element, move all elements ahead of the index, back by one element.
				if (index < (_count - 1))
					Memory::copyType(&_first[index], _first[index + 1], (_count - (index + 1)));

				_count--;
				return true;
			}

			inline bool contains(T item) {
				for (uint32_t i = 0; i < _count; i++) {
					if (item == _first[i])
						return true;
				}

				return false;
			}

			inline void clear() override {
				_count = 0;
			}

			inline bool isEmpty() override {
				return _count == 0;
			}

			inline T& operator[] (uint32_t index) {
				if (index >= _count || index < 0)
					throw IndexOutOfRangeExeption(index, _count);

				return _first[index];
			}

		private:
			T* _first;
			T* _end;
			uint32_t _capacity;
			FerrousAllocator* _allocator;
		};
	}
}