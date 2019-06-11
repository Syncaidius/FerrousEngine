#include "file.h"

File::Exception::Exception(const char* msg, const Result result) {
	_msg = msg;
	_result = result;
}