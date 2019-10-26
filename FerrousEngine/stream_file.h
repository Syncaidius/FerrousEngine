#pragma once
#include "stream.h"
#include "macros.h"
#include <fstream>

namespace fe {
	enum class FileStreamFlags : uint8_t {
		None = 0,

		/* Creates the file if it is missing, otherwise the existing file will be opened. */
		Create = 1,

		/* Once the file is opened, move to the end of the file to write new data.*/
		Append = 2,

		/* Open the file in binary mode. Default is text mode.*/
		Binary = 4,
	};

	GEN_ENUM_CLASS_OPERATORS(FileStreamFlags);

	class FileStream : public Stream {
	public:
		FileStream();

		FileStream(const FeString& path,
			const FileStreamFlags flags,
			bool canRead,
			bool canWrite,
			const UtfEncoding encoding = UtfEncoding::Auto);

		~FileStream();
		inline UtfEncoding getEncoding() { return _encoding; }
		inline const FeString& getPath() const { return _path; }

		UtfEncoding getEncoding() const override;
		void close() override;
		void seekRead(StreamSeek origin, int32_t num_bytes) override;
		void setReadPos(size_t pos) override;
		size_t getReadPos() override;

		void seekWrite(StreamSeek origin, int32_t num_bytes) override;
		void setWritePos(size_t pos) override;
		size_t getWritePos() override;

		void setSize(size_t size) override;
		size_t getSize() override;

		std::istream& getReadStream() override;
		std::ostream& getWriteStream() override;

		void readBytes(char* dest, size_t num_bytes) override;
		void writeBytes(const char* bytes, size_t num_bytes) override;

		FileStream(const FileStream&) = delete;
		inline FileStream& operator =(FileStream&& other) {
			return other;
		}
	private:
		UtfEncoding _encoding;
		const FeString _path;
		FileStreamFlags _flags;
		std::fstream _stream;

		UtfEncoding detectEncoding();
	};
}