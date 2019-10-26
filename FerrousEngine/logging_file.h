#pragma once
#include "logging.h"
#include "stream_file.h"
#include "stream_text_writer.h"

namespace fe {
	class FileLogOutput : public LogOutputBase {
	public:
		FileLogOutput(const FeString& filePath);
		~FileLogOutput();

	protected:
		friend class Logger;

		void write(const FeString& msg, const Color& color) override;
		void writeLine(const FeString& msg, const Color& color) override;
		void clear() override;
		void close() override;
	private:
		FileStream _stream;
		TextStreamWriter _writer;
	};
}