#include "logging_file.h";
#include <Windows.h>
#include <iostream>

namespace fe {
	FileLogOutput::FileLogOutput(const FeString& filename) {
		_stream = FileStream(filename, FileStreamFlags::Create, false, true);
		_writer = TextStreamWriter(&_stream, Memory::get());
	}

	FileLogOutput::~FileLogOutput() {
		FreeConsole();
	}


	void FileLogOutput::write(const FeString& msg, const Color& color) {
		_writer.write(msg);
	}

	void FileLogOutput::writeLine(const FeString& msg, const Color& color) {
		_writer.writeLine(msg);
	}

	void FileLogOutput::clear() {
		// TODO truncate file, perhaps.
	}

	void FileLogOutput::close() {
		_stream.close();
	}
}