#pragma once
#include "key_value_pair.h"
#include "collection.h"

namespace fe {
	namespace collections {
		template <typename K, typename V>
		struct Bucket {
		public:
			// Next bucket with the same key
			Bucket<K,V>* next;
			V value;

			Bucket(const K& key, const V& value) :
				_key(key), value(value), next(nullptr) {
			}

			const K& getKey() const {
				return _key;
			}

		private:
			K _key;
		};

		/* Default hash function class */
		template <typename K>
		struct DefaultKeyHash {
			uint64_t operator()(const K& key, const uint32_t tableSize) const
			{
				return reinterpret_cast<const uint64_t&>(key) % tableSize;
			}
		};

		/* HashID map class template */
		template <typename K, typename V, typename F = DefaultKeyHash<K>>
		class Dictionary {
		public:
			Dictionary(uint32_t tableSize = 4, FerrousAllocator* allocator = Memory::get()) {
				_tableSize = tableSize;
				_allocator = allocator;
				_table = _allocator->allocType<Bucket<K, V>*>(_tableSize);
				Memory::ZeroType<Bucket<K,V>*>(_table, _tableSize);
			}

			~Dictionary() {
				destroyTable();
			}

			bool containsKey(const K& key) {
				HashID hashValue = _hashFunc(key, _tableSize);
				Bucket<K, V>* entry = _table[hashValue];

				while (entry != nullptr) {
					if (entry->getKey() == key)
						return true;

					entry = entry->next;
				}

				return false;
			}

			V get(const K& key) {
				HashID hashValue = _hashFunc(key, _tableSize);
				uint32_t bucketID = hashValue % _tableSize;
				Bucket<K, V>* entry = _table[bucketID];

				while (entry != nullptr) {
					if (entry->getKey() == key) 
						return entry->value;

					entry = entry->next;
				}

				throw "Key not found.";
			}

			bool tryGet(const K& key, V& value) {
				HashID hashValue = _hashFunc(key, _tableSize);
				uint32_t bucketID = hashValue % _tableSize;
				Bucket<K, V>* entry = _table[bucketID];

				while (entry != nullptr) {
					if (entry->getKey() == key) {
						value = entry->value;
						return true;
					}
					entry = entry->next;
				}
				return false;
			}

			void put(const K& key, const V& value) {
				HashID hashValue = _hashFunc(key, _tableSize);
				uint32_t bucketID = hashValue % _tableSize;
				Bucket<K, V>* prev = nullptr;
				Bucket<K, V>* entry = _table[bucketID];

				while (entry != nullptr && !(entry->getKey() == key)) {
					prev = entry;
					entry = entry->next;
				}

				if (entry == nullptr) {
					entry = _allocator->allocType<Bucket<K, V>>();
					new (entry) Bucket<K, V>(key, value);

					if (prev == nullptr)
						_table[bucketID] = entry; // insert as first entry in bucket.
					else
						prev->next = entry;
				}
				else {
					// just update the value
					entry->value = value;
				}
			}

			bool remove(const K& key) {
				HashID hashValue = _hashFunc(key, _tableSize);
				uint32_t bucketID = hashValue % _tableSize;
				Bucket<K, V>* prev = nullptr;
				Bucket<K, V>* entry = _table[bucketID];

				while (entry != nullptr ) {
					// Delete entry if key found.
					if (entry->getKey() == key) {
						if (prev == nullptr)
							_table[bucketID] = entry->next;
						else
							prev->next = entry->next;

						_allocator->dealloc(entry);
						return true;
					}

					prev = entry;
					entry = entry->next;
				}

				return false;
			}

			void ensureCapacity(uint32_t capacity) {
				if (_tableSize >= capacity)
					return;

				uint32_t newSize = _tableSize * 2;
				if (newSize < capacity)
					newSize = capacity;

				Bucket<K, V>* newTable = _allocator->allocType<Bucket<K, V>*>(newSize);

				// Iterate over all buckets in old table and put them into the new one.
				for (uint32_t i = 0; i < _tableSize; i++) {
					Bucket<K, V>* entry = _table[i];
					K key = entry->getKey();
					HashID hashValue = _hashFunc(key, newSize);
					uint32_t bucketID = hashValue % newSize;

					// Put the bucket into the new table
					newTable[bucketID] = entry;
				}

				// Destroy old table and store new one.
				destroyTable();
				_tableSize = newSize;
				_table = newTable;
			}

			inline V& operator[] (const K& key) {
				HashID hashValue = _hashFunc(key, _tableSize);
				uint32_t bucketID = hashValue % _tableSize;
				Bucket<K, V>* entry = _table[bucketID];

				while (entry != nullptr) {
					if (entry->getKey() == key)
						return entry->value;

					entry = entry->next;
				}

				throw "Key not found.";
			}

		private:
			typedef uint64_t HashID;

			// hash table
			Bucket<K, V>** _table;
			uint32_t _tableSize;
			FerrousAllocator* _allocator;
			F _hashFunc;

			void destroyTable() {
				// Deallocate nodes
				for (uint32_t i = 0; i < _tableSize; i++) {
					Bucket<K, V>* node = _table[i];
					while (node != nullptr) {
						Bucket<K, V>* prev = node;
						node = node->next;
						_allocator->dealloc(prev);
					}

					_table[i] = nullptr;
				}

				_allocator->dealloc(_table);
			}
		};
	}
}