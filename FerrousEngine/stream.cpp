#include "stream.h"

namespace fe {
	Stream::Stream(bool canRead, bool canWrite) :
		_canRead(canRead),
		_canWrite(canWrite) {
		_isOpen = true;
	}

	Stream::~Stream() {

	}
}