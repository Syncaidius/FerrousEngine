#pragma once
#include "util_export.h";
#include "memory_allocator.h"
#include <iostream>

MemoryAllocator mem_allocator(8192);
void* operator new(std::size_t n) throw(std::bad_alloc)
{
#if _DEBUG
	void* p = mem_allocator.alloc(n);
	std::cout << "Allocating block of " << n << std::endl;
	return p;
#else
	return mem_allocator.alloc(n);
#endif
}

void operator delete(void* p) throw()
{
#if _DEBUG
	std::cout << "Releasing memory at " << p << std::endl;
#endif

	mem_allocator.release(p);
}