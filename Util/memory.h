#pragma once
#include "stdafx.h"
#include "util.h";

class FERROUS_UTIL_API Memory {
public:
	struct Block {
		size_t size;
		Block* next;
		uint8_t adjustment; /* Adjustment must always be last member.*/

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

	void* alloc(const size_t size_bytes);

	/*Allocates a new block of memory capable of fitting num_elements of type T. */
	template<typename T> T* alloc(const size_t num_elements = 1) {
		void* mem = alloc(sizeof(T) * num_elements);
		memset(mem, 0, sizeof(T));
		return reinterpret_cast<T*>(mem);
	}

	void realloc(void*& target, const size_t old_num_bytes, const size_t num_bytes);

	/*Allocates a new block of memory capable of fitting num_elements of type T, then copies the old one to it. Once complete, the memory of the old array is released for reuse.
	Updates the oldArray pointer to point to the newly-resized array.*/
	template<typename T> void realloc(T*& target, const size_t old_num_elements, const size_t num_elements) {
		realloc(target, sizeof(T) * old_num_elements, sizeof(T) * num_elements);
	}

	/* Allocates a block of aligned memory.*/
	void* allocAligned(const size_t size_bytes, uint8_t alignment);

	Stack* allocStack(const size_t size_bytes, const uint8_t alignment);

	/* Gets an aligned pointer within the specified block of memory. */
	void* align(void* p, uint8_t alignment, size_t start_offset = 0);

	void dealloc(void* p);

	void reset(void);

	void defragment(void);

	Memory::Block* getFreeList() {
		return _free;
	}

	size_t getAllocated() { return _allocated; }

	size_t getOverhead() { return _overhead; }

	static Memory* get() { return _allocator; }

private:
	static Memory* _allocator;
	Memory();
	~Memory();

	void* _start;
	Block* _free;
	size_t _allocated;
	size_t _overhead;

	void mergeSort(Block** headRef);
	Block* sortedMerge(Block* a, Block* b);
	void frontBackSplit(Block* source, Block** frontRef, Block** backRef);

	inline Block* getHeader(void* p) {
		// Header is located behind the pointer's address.#
		size_t adjustment_loc = HEADER_SIZE - offsetof(struct Block, adjustment);
		uint8_t* adjustment = reinterpret_cast<uint8_t*>((char*)p - adjustment_loc);
		return reinterpret_cast<Block*>((char*)p - *adjustment - HEADER_SIZE);
	}
};