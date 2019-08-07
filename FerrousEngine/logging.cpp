#include "logging.h"
#include <time.h>
#include <stdlib.h>

namespace fe {
	Logger::Logger(uint16_t initial_slot_count) {
		_output_slot_count = initial_slot_count;
		_outputs = Memory::get()->allocType<LogOutputHandle>(_output_slot_count);
		Memory::ZeroType<LogOutputHandle>(_outputs, _output_slot_count);
	}

	Logger::~Logger() {
		LogOutputHandle* end = _outputs;
		end += _output_slot_count;
		for (LogOutputHandle* i = _outputs; i != end; i++) {
			if (i != nullptr)
				i->output->close();
		}
	}

	void Logger::addOutput(LogOutputBase * output) {
		// Try and find an existing output slot before resizing the array.
		LogOutputHandle* end = _outputs;
		end += _output_slot_count;

		LogOutputHandle* i = nullptr;
		for (i = _outputs; i != end; i++) {
			if (i->output == nullptr) {
				i->output = output;
				return;
			}
		}

		// If we've reached here, no slots were found. Expand!
		// TODO use vector?
		uint64_t _next = _output_slot_count++;
		Memory::get()->reallocType(_outputs, _output_slot_count);

		// Jump straight to the slot we just added.
		i = _outputs;
		i += _next;
		*i->output = *output;
	}

	void Logger::clear() {
		LogOutputHandle* end = _outputs;
		end += _output_slot_count;
		for (LogOutputHandle* i = _outputs; i != end; i++) {
			if (i != nullptr)
				i->output->clear();
		}
	}

	void Logger::write(const FeString & msg, const Color & color) {
		LogOutputHandle* end = _outputs;
		end += _output_slot_count;
		for (LogOutputHandle* i = _outputs; i != end; i++) {
			if (i != nullptr)
				i->output->write(msg, color);
		}
	}

	void Logger::writeLine(const FeString & msg, const Color & color) {
		FeString result = "["_fe + FeString::dateTime(L"%X") + "] "_fe + msg; // TODO do we need a proper FeString-builder/stream for situations like this?
		LogOutputHandle* end = _outputs;
		end += _output_slot_count;
		for (LogOutputHandle* i = _outputs; i != end; i++) {
			if (i != nullptr)
				i->output->writeLine(result, color);
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