#include "logging.h"
#include <time.h>
#include <stdlib.h>

namespace fe {
	Logger::Logger(uint16_t initial_slot_count) {
		_output_slot_count = initial_slot_count;
		_outputs = List<LogOutputBase*>();
	}

	Logger::~Logger() {
		for (LogOutputBase* l : _outputs) {
			if (l != nullptr)
				l->close();
		}
	}

	void Logger::addOutput(LogOutputBase * output) {
		_outputs.add(output);
	}

	void Logger::clear() {
		for (LogOutputBase* l : _outputs) {
			if (l != nullptr)
				l->clear();
		}
	}

	void Logger::write(const FeString & msg, const Color & color) {
		for (LogOutputBase* l : _outputs) {
			if (l != nullptr)
				l->write(msg, color);
		}
	}

	void Logger::writeLine(const FeString & msg, const Color & color) {
		FeString result = "["_fe + FeString::dateTime(L"%X") + "] "_fe + msg; // TODO do we need a proper FeString-builder/stream for situations like this?
		for (LogOutputBase* l : _outputs) {
			if (l != nullptr)
				l->writeLine(result, color);
		}
	}

	void Logger::writeF(const FeString & str, const Color & color, ...) {
		va_list args;
		va_start(args, color);
		FeString result = FeString::format(str, args);
		va_end(args);
		write(result, color);
	}

	void Logger::writeF(const FeString & str, ...) {
		va_list args;
		va_start(args, str);
		FeString result = FeString::format(str, args);
		va_end(args);
		write(result, Color::white);
	}

	void Logger::writeLineF(const FeString & str, const Color & color, ...) {
		va_list args;
		va_start(args, color);
		FeString result = FeString::format(str, args);
		va_end(args);
		writeLine(result, color);
	}

	void Logger::writeLineF(const FeString & str, ...) {
		va_list args;
		va_start(args, str);
		FeString result = FeString::format(str, args);
		va_end(args);
		writeLine(result, Color::white);
	}
}