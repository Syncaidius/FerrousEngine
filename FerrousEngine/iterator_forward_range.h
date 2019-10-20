#pragma once
namespace fe {
	namespace collections {
		// AN
		template<typename T>
		class ForwardRangeIterator {
		public:
			// The 5 std::iterator properties, since they're no longer provided by std::iterator in C++ 17.
			// These describe how and what the iterator provides.
			using value_type = T;
			using reference = T &;
			using iterator_category = std::forward_iterator_tag; // See: https://en.cppreference.com/w/cpp/iterator/iterator_tags
			using pointer = T *;
			using difference_type = void;

			explicit ForwardRangeIterator(T* data, uint32_t index) {
				_data = data + index;
			}

			ForwardRangeIterator& operator++() {
				_data++;
				return *this;
			}

			T& operator*() {
				return *_data;
			}

			bool operator !=(const ForwardRangeIterator& other) const {
				return _data != other._data;
			}

			bool operator ==(const ForwardRangeIterator& other) const {
				return _data == other._data;
			}

		private:
			T* _data;
		};
	}
}