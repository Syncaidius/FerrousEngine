#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <iostream>
#include <allocation.h>

//void* operator new(size_t n) throw(std::bad_alloc)
//{
//	return Memory::get()->alloc(n);
//}
//
//void operator delete(void* p) throw()
//{
//	Memory::get()->dealloc(p);
//}