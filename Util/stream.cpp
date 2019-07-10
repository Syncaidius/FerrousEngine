#include "stream.h"

Stream::Stream(bool canRead, bool canWrite) :
	_canRead(canRead),
	_canWrite(canWrite) {
	_isOpen = true;
}

Stream::~Stream() {
	
}