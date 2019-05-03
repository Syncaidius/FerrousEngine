#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <iostream>
#include <Util/memory_allocator.h>

//MemoryAllocator* mem_allocator = MemoryAllocator::get();
//void* operator new(std::size_t n) throw(std::bad_alloc)
//{
//#if _DEBUG
//	void* p = mem_allocator->alloc(n);
//	std::cout << "Allocating block of " << n << " bytes" << std::endl;
//	return p;
//#else
//	return mem_allocator.alloc(n);
//#endif
//}
//
//void operator delete(void* p) throw()
//{
//#if _DEBUG
//	std::cout << "Releasing memory at " << p << std::endl;
//#endif
//
//	mem_allocator->dealloc(p);
//}