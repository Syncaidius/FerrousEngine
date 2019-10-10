#pragma once
#include "collection.h"
#include "allocation.h"
#include "exceptions.h"

namespace fe {
	namespace collections {
		template<typename T>
		class List : public Collection<T> {

		public:
			class iterator : public BaseIterator<T> {
				// TODO implement
			};

			inline List(uint32_t initialCapacity = 1, FerrousAllocator* allocator = Memory::get()) {
				assert(initialCapacity > 0);

				_allocator = allocator;
				_capacity = initialCapacity;
				_first = _allocator->allocType<T>(_capacity);
			}

			List<T>& operator=(const List<T>&) = delete;
			List<T> operator=(const List<T>&) = delete;

			iterator begin() override {
				return iterator(this, 0);
			}

			iterator end() override {
				return iterator(this, _count);
			}

			inline bool insert(T item, uint32_t index) {
				if(index < 0 || index > _count)
					throw IndexOutOfRangeExeption(index, _count);

				// Ensure capacity.
				// We're inserting between existing items.
				if (index < _count) {
					T* dest = _first;

					if (_count == _capacity) {
						_capacity *= 2;
						dest = _allocator->allocType<T>(_capacity);
					}

					if (index == 0) {
						Memory::copyType(&dest[1], _first, _count);
					}
					else {
						// Split elements into two parts, leaving a 1-element gap, for the new item to be inserted.
						Memory::copyType(dest, _first, index);
						Memory::copyType(&dest[index + 1], &_first[index], _count - index);
					}

					// Deallocate old memory, if new was allocated.
					if (_first != dest) {
						_allocator->dealloc(_first);
						_first = _dest;
					}

					_first[index] = item;
					return true;
				}
				else { // We're appending to the end (i.e. equal to _count).
					return add(item);
				}
			}

			inline bool insertCollection(Collection<T> items, uint32_t insertIndex, uint32_t startIndex, uint32_t count) {
				if (startIndex + count > items.count())
					throw IndexOutOfRangeExeption(sstartIndex + count, items.count());

				uint32_t capRequired = _count + count;
				if (index < 0 || index > _count)
					throw IndexOutOfRangeExeption(index, _count);

				if (insertIndex < _count) {

				}
				else {
					addCollection(items, startIndex, count);
				}
			}

			inline bool add(T item) override {
				if (_count == _capacity) {
					_capacity *= 2;

					// Copy existing elements to new location.
					T* newMem = _allocator->allocType<T>(_capacity);
					Memory::copyType(newMem, _first, _count);
					_allocator->dealloc(_first);
					_first = newMem;
				}

				_first[_count++] = item;
				return true;
			}

			inline void addCollection(Collection<T>& collection, uint32_t startIndex, uint32_t count) {
				int required = _count + collection.count();

				if (required > _capacity) {
					_capacity *= 2;

					// Add whatever we're short by, to the new capacity.
					if (_capacity < required)
						_capacity += required - _capacity;

					T* newMem = _allocator->allocType<T>(_capacity);
					Memory::copyType(newMem, _first, _count);
					_allocator->dealloc(_first);
					_first = newMem;
				}

				// Copy items from collection to the end of the current list.
				collection.copyTo(&_first[_count], startIndex, count);
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
			}

			inline void copyTo(T* dest, uint32_t startIndex, uint32_t copyCount) override {
				if (startIndex >= _count || startIndex < 0)
					throw IndexOutOfRangeExeption(startIndex, _count);

				if (startIndex + count > _count)
					throw IndexOutOfRangeExeption(startIndex + copyCount, _count);

				Memory::copyType(dest, &_first[startIndex], copyCount);
			}

			inline bool contains(T item) {
				for (uint32_t i = 0; i < _count; i++) {
					if (item == _first[i])
						return true;
				}

				return false;
			}

			// Removes all empty capacity from the end of the list, if any.
			inline void trim() {
				if (_capacity == _count)
					return 0;

				T * trimmedMem = _allocator->allocType<T>(_count);
				Memory::copyType(trimmedMem, _first, _count);
				_allocator->dealloc(_first);
				_first = trimmedMem;
				_capacity = _count;
			}

			inline void clear() override {
				_count = 0;
			}

			inline bool isEmpty() override {
				return _count == 0;
			}

			inline uint32_t capacity() {
				return _capacity;
			}			

			inline T& operator[] (uint32_t index) {
				if (index >= _count || index < 0)
					throw IndexOutOfRangeExeption(index, _count);

				return _first[index];
			}

		private:
			T* _first;
			uint32_t _capacity;
			FerrousAllocator* _allocator;
		};
	}
}