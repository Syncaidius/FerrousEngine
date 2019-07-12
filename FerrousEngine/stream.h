#pragma once
#include "allocation.h"
#include <iostream>
#include "strings.h"

enum class StreamSeek : uint8_t {
	/* From the beginning of the stream. */
	Beginning = 0,

	/* From the current read or write position of the stream.*/
	Current = 1,

	/* From the end of the stream. */
	End = 2,
};

class Stream {
public:
	Stream(bool canRead, bool canWrite);
	~Stream();

	inline bool isOpen() { return _isOpen; }

	inline bool canRead() { return _canRead; }
	inline bool canWrite() { return _canWrite; }

	virtual UtfEncoding getEncoding() const = 0;
	virtual void close() = 0;
	virtual void seekRead(StreamSeek origin, int32_t num_bytes) = 0;
	virtual void setReadPos(size_t pos) = 0;
	virtual size_t getReadPos() = 0;

	virtual void seekWrite(StreamSeek origin, int32_t num_bytes) = 0;
	virtual void setWritePos(size_t pos) = 0;
	virtual size_t getWritePos() = 0;

	virtual void setSize(size_t size) = 0;
	virtual size_t getSize() = 0;

	virtual std::istream& getReadStream() = 0;
	virtual std::ostream& getWriteStream() = 0;

	virtual void readBytes(char* dest, size_t num_bytes) = 0;
	virtual void writeBytes(const char* bytes, size_t num_bytes) = 0;
protected:
	bool _isOpen;
	const bool _canRead;
	const bool _canWrite;
};

#define DEFINE_STREAM_ERROR(ClassName, Message) \
	class ClassName : public std::exception { \
		Stream* _stream; \
		public: \
		inline ClassName(Stream* stream, const char* msg = Message) : exception(msg) { \
			_stream = stream; \
		} \
		inline const Stream* getStream() const throw() { return _stream; } \
	};

DEFINE_STREAM_ERROR(StreamClosedError, "Operation failed. The stream was already closed.")
DEFINE_STREAM_ERROR(StreamReadAccessError, "The stream does not provide read access.")
DEFINE_STREAM_ERROR(StreamWriteAccessError, "The stream does not provide write access.");
DEFINE_STREAM_ERROR(EndOfStreamError, "Cannot read beyond the end of the stream.");