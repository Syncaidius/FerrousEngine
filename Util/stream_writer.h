#pragma once
#include "stream.h";

class FERROUS_UTIL_API StreamWriter {
public:

	StreamWriter(Stream* stream);
	virtual ~StreamWriter();

	inline Stream* getStream() { return _stream; }

protected:
	Stream* _stream;
};