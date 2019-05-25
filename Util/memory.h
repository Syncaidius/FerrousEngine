#pragma once
#include "stdafx.h"
#include "util.h";

class FERROUS_UTIL_API Memory {
public:
	struct Block {
		size_t size;
		Block* next;
		uint16_t ref_count;
		uint8_t adjustment;

		/* Block data follows the last member of Block. */
	};

	class Stack {
	public:
		/* Allocates the specified amount of memory.*/
		void* alloc(size_t num_bytes);

		/* De-allocate all allocated memory in the stack to the specified pointer.*/
		void revertTo(void* p);

		/* Resets the stack back to it's starting address.*/
		void reset(void);
	private:
		friend class Memory;
		Stack(Memory* mem_parent, void* mem);
		~Stack();

		Memory* _parent;
		void* _start;
		uintptr_t _end;
		void* _pos;
	};

	const static size_t HEADER_SIZE = sizeof(Block);
	const static size_t PAGE_SIZE = 8912;

	/*Allocates and zeroes a new block of memory capable of fitting the requested number of bytes. */
	void* alloc(const size_t size_bytes);

	/*Allocates, but does not zero, a new block of memory capable of fitting the requested number of bytes. */
	inline void* allocFast(const size_t size_bytes);

	/*Allocates and zeroes a new block of memory capable of fitting num_elements of type T. */
	template<typename T> T* allocType(const size_t num_elements = 1) {
		void* mem = alloc(sizeof(T) * num_elements);
		return reinterpret_cast<T*>(mem);
	}

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
	template<typename T> void reallocType(T*& target, const size_t old_num_elements, const size_t num_elements) {
		realloc((void*&)target, sizeof(T) * old_num_elements, sizeof(T) * num_elements);
	}

	void copy(void* dest, const void* src, const size_t num_bytes);

	/* Copies memory from the source, to the destination. */
	template<typename T> inline void copyType(T* dest, const T* src, const size_t num_elements) {
		copy(dest, src, sizeof(T) * num_elements);
	}

	/* Allocates a block of aligned memory.*/
	void* allocAligned(const size_t size_bytes, uint8_t alignment);

	Stack* allocStack(const size_t size_bytes, const uint8_t alignment);

	/* Gets an aligned pointer within the specified block of memory. */
	inline void* align(void* p, uint8_t alignment, size_t start_offset = 0);

	void dealloc(void* p);

	void reset(void);

	void defragment(void);

	Memory::Block* getFreeList() {
		return _free;
	}

	size_t getAllocated() const { return _allocated; }

	size_t getOverhead() const { return _overhead; }

	static inline Memory* get() { return _allocator; }

private:
	static Memory* _allocator;
	Memory();
	~Memory();

	void* _start;
	Block* _free;
	size_t _allocated;
	size_t _overhead;
	size_t _count_free;
	size_t _count_allocated;

	bool tryMerge(Block* prev, Block* cur);
	void mergeSort(Block** headRef);
	Block* sortedMerge(Block* a, Block* b);
	void frontBackSplit(Block* source, Block** frontRef, Block** backRef);

	inline Block* getHeader(void* p);
};