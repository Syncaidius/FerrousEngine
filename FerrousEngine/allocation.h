#pragma once
#include "stdafx.h"
class StackAllocator;

class FerrousAllocator {
public:
	/*Allocates and zeroes a new block of memory capable of fitting the requested number of bytes. */
	virtual void* alloc(const size_t size_bytes, const uint8_t alignment) = 0;

	/*Allocates and a new block of memory capable of fitting num_elements of type T. */
	template<typename T> T* allocType(const size_t num_elements = 1) {
		void* mem = alloc(sizeof(T) * num_elements, alignof(T));
		return static_cast<T*>(mem);
	}

	virtual void dealloc(void* p) = 0;

	/* Calls the object's destructor and dallocates its memory.*/
	template<typename T>
	void deallocType(T* obj_location) {
		obj_location->~T();
		dealloc(obj_location);
	}

	virtual void reset() = 0;

	/* Gets amount of memory allocated/reserved for the current allocator. */
	virtual size_t getCapacity() = 0;

	/* Increases reference counter of allocated memory.*/
	virtual void ref(void* p) = 0;

	virtual void deref(void* p) = 0;

protected:
	uint8_t alignForwardAdjustment(const void* p, uint8_t alignment);
	void* alignForward(const void* p, uint8_t alignment);
};

class Memory : public FerrousAllocator {
public:
	struct Page;

	class Block{
		friend class Memory;
		size_t _size;
		union {
			Block* _next;
			struct Info {
				uint16_t _ref_count;
				uint8_t _reserved1;
				uint8_t _reserved2;
				uint8_t _reserved3;
				uint8_t _reserved4;
				uint8_t _reserved5;
				uint8_t _adjustment; /* Should always be the last member. Not always used when followed by adjusted (aligned) data.*/
			} _info;
		};

		/* Block data follows. */
	};

	class Page {
		friend class Memory;
		Page* _next;

		/* Page blocks follows. */
	};

	const static size_t BLOCK_HEADER_SIZE = sizeof(Block);
	const static size_t PAGE_HEADER_SIZE = sizeof(Page);
	const static size_t PAGE_SIZE = 8192;
	const static size_t PAGE_MIN_OVERHEAD = BLOCK_HEADER_SIZE + PAGE_HEADER_SIZE;
	const static size_t PAGE_FREE_SIZE = PAGE_SIZE - PAGE_MIN_OVERHEAD;

	/*Allocates and zeroes a new block of memory capable of fitting the requested number of bytes. */
	void* alloc(const size_t size_bytes, uint8_t alignment) override;

	/* Increments the reference count of a block of memory. If the reference count hits 0, it will automatically be deallocated. */
	void ref(void* p) override;

	/* Dereferences a block of memory. If the reference count hits 0, it will automatically be deallocated. */
	void deref(void* p) override;

	void realloc(void*& target, const size_t num_bytes, uint8_t alignment);

	/*Allocates a new block of memory capable of fitting num_elements of type T, then copies the old one to it. Once complete, the memory of the old array is released for reuse.
	Updates the oldArray pointer to point to the newly-resized array.*/
	template<typename T>
	void reallocType(T*& target, const size_t num_elements) {
		realloc((void*&)target, sizeof(T) * num_elements, alignof(T));
	}

	static void copy(void* dest, const void* src, const size_t num_bytes);

	/* Copies memory from the source, to the destination. */
	template<typename T> inline static void copyType(T* dest, const T* src, const size_t num_elements) {
		copy(dest, src, sizeof(T) * num_elements);
	}

	void dealloc(void* p) override;

	void reset() override;

	/* defragments the specified number of pages. Continues from the last defragmented page.
	If the last page is reached, it will wrap around to the first page and start over,
	unless the total number of pages is less than the specified amount.*/
	void defragment(size_t max_pages);

	/* Gets amount of memory allocated/reserved for the current allocator. */
	size_t getCapacity() override;

	/* Outputs debug information about the memory structure into stdout. */
	void outputDebug();

	static inline Memory* get() { return _allocator; }

	static inline void Zero(void* p, uint8_t val, size_t num_bytes = 1) {
		memset(p, val, num_bytes);
	}

	template<typename T> 
	static inline void ZeroType(T* dest, const size_t num_elements = 1) {
		memset(dest, 0, sizeof(T) * num_elements);
	}


private:
	static Memory* _allocator;
	Memory();
	~Memory();

	Page* _pages;
	Block* _blocks;
	Page* _page_to_defrag; /* Next page to be defragged. */
	size_t _page_count;

	bool canMerge(Block* prev, Block* cur);
	void mergeSort(Block** headRef);
	Block* sortedMerge(Block* a, Block* b);
	void frontBackSplit(Block* source, Block** frontRef, Block** backRef);

	inline Block* getBlockHeader(const void* p);
	inline Block* makePageBlock(Page* p);
	Block* newPage(void);
};