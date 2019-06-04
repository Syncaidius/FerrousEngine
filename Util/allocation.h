#pragma once
#include "stdafx.h"
#include "util.h";
class FERROUS_UTIL_API StackAllocator;

class FERROUS_UTIL_API Allocator {
public:
	/*Allocates and zeroes a new block of memory capable of fitting the requested number of bytes. */
	virtual void* alloc(const size_t size_bytes) = 0;

	/*Allocates and zeroes a new block of memory capable of fitting num_elements of type T. */
	template<typename T> T* allocType(const size_t num_elements = 1) {
		void* mem = alloc(sizeof(T) * num_elements);
		return reinterpret_cast<T*>(mem);
	}

	StackAllocator* allocStack(const size_t size_bytes, const uint8_t alignment = 1);

	virtual void dealloc(void* p) = 0;

	virtual size_t getTotalAllocated() = 0;
};

class FERROUS_UTIL_API Memory : public Allocator {
public:
	struct Page;

	struct Block {
	public:
		inline size_t getSize() { return _size; }
		inline Block* getNext() { return _next; }
		inline uint16_t getRefCount() { return _ref_count; }
	private:
		friend class Memory;
		size_t _size;
		Block* _next;
		Page* _page; // TODO store page ID instead. We can cut 4 bytes off here.
		uint16_t _ref_count;
		uint8_t _adjustment;

		/* Block data follows the last member of Block. */
	};

	struct Page {
	public:
		Memory::Block* getFreeList() {
			return _blocks;
		}

		inline Page* getNext() {
			return _next;
		}

		inline size_t getAllocated() const {
			return _allocated;
		}

		inline size_t getOverhead() const {
			return _overhead;
		}

		inline size_t getBlocksAllocated() const {
			return _blocks_allocated;
		}

		inline size_t getBlocksFree() const {
			return _blocks_free;
		}

	private:
		friend class Memory;
		Page* _next;
		Block* _blocks;
		size_t _allocated;
		size_t _overhead;
		size_t _blocks_free;
		size_t _blocks_allocated;
	};

	const static size_t PAGE_LOOKUP_TABLE_BASE_SIZE = 1024; // In bytes.
	const static size_t BLOCK_HEADER_SIZE = sizeof(Block);
	const static size_t PAGE_HEADER_SIZE = sizeof(Page);
	const static size_t PAGE_SIZE = 8192;
	const static size_t PAGE_MIN_OVERHEAD = BLOCK_HEADER_SIZE + PAGE_HEADER_SIZE;
	const static size_t PAGE_FREE_SIZE = PAGE_SIZE - PAGE_MIN_OVERHEAD;

	/*Allocates and zeroes a new block of memory capable of fitting the requested number of bytes. */
	void* alloc(const size_t size_bytes);

	/*Allocates, but does not zero, a new block of memory capable of fitting the requested number of bytes. */
	void* allocFast(const size_t size_bytes);

	template<typename T> inline T* allocTypeFast(const size_t num_elements = 1) {
		void* mem = allocFast(sizeof(T) * num_elements);
		return reinterpret_cast<T*>(mem);
	}

	/* Increments the reference count of a block of memory. If the reference count hits 0, it will automatically be deallocated. */
	void ref(void* p);

	/* Dereferences a block of memory. If the reference count hits 0, it will automatically be deallocated. */
	void deref(void* p);

	void realloc(void*& target, const size_t old_num_bytes, const size_t num_bytes);

	/*Allocates a new block of memory capable of fitting num_elements of type T, then copies the old one to it. Once complete, the memory of the old array is released for reuse.
	Updates the oldArray pointer to point to the newly-resized array.*/
	template<typename T>
	void reallocType(T*& target, const size_t old_num_elements, const size_t num_elements) {
		realloc((void*&)target, sizeof(T) * old_num_elements, sizeof(T) * num_elements);
	}

	void copy(void* dest, const void* src, const size_t num_bytes);

	/* Copies memory from the source, to the destination. */
	template<typename T> inline void copyType(T* dest, const T* src, const size_t num_elements) {
		copy(dest, src, sizeof(T) * num_elements);
	}

	/* Allocates a block of aligned memory.*/
	void* allocAligned(const size_t size_bytes, uint8_t alignment);

	void dealloc(void* p);

	/* Calls the object's destructor and dallocates its memory.*/
	template<typename T>
	void deallocType(T* obj_location) {
		obj_location->~T();
		dealloc(obj_location);
	}

	void reset(bool release_pages);

	/* defragments the specified number of pages. Continues from the last defragmented page.
	If the last page is reached, it will wrap around to the first page and start over,
	unless the total number of pages is less than the specified amount.*/
	void defragment(int iterations);

	Memory::Page* getFirstPage() { return _pages; }

	/* Gets the total number of allocated bytes. */
	size_t getTotalAllocated();

	inline size_t getOverhead() const { return _total_overhead; }

	inline size_t getPageCount() const { return _total_pages; }

	inline size_t getAllocBlockCount() const { return _total_alloc_blocks; }

	inline size_t getFreeBlockCount() const { return _total_free_blocks; }

	static inline Memory* get() { return _allocator; }

private:
	static Memory* _allocator;
	Memory();
	~Memory();

	Page* _pages;
	Page* _page_to_defrag; /* Next page to be defragged. */
	size_t _total_alloc;
	size_t _total_overhead;
	size_t _total_free_blocks;
	size_t _total_alloc_blocks;
	size_t _total_pages;

	/* Gets an aligned pointer within the specified block of memory. */
	inline void* align(void* p, uint8_t alignment, size_t start_offset = 0);

	bool tryMerge(Page* page, Block* prev, Block* cur);
	void mergeSort(Block** headRef);
	Block* sortedMerge(Block* a, Block* b);
	void frontBackSplit(Block* source, Block** frontRef, Block** backRef);

	inline Block* getHeader(void* p);
	Page* newPage(void);
	void resetPage(Page* p);
};

class FERROUS_UTIL_API StackAllocator : public Allocator {
public:
	~StackAllocator();

	/* Allocates the specified amount of memory.*/
	void* alloc(size_t num_bytes);

	/* De-allocate all allocated memory in the stack to the specified pointer.*/
	void revertTo(void* p);

	void dealloc(void* p);

	/* Resets the stack back to it's starting address.*/
	void reset(void);

	size_t getTotalAllocated();
private:
	friend class Allocator;
	StackAllocator(Allocator* parent, void* mem);

	Allocator* _parent;
	void* _mem;
	uintptr_t _end;
	void* _pos;
};