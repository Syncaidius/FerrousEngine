#include "logging.h"
#include <time.h>
#include <stdlib.h>

namespace fe {
	Logger::Logger() {
		_outputs = List<LogOutputBase*>();
	}

	Logger::~Logger() {
		for (LogOutputBase* l : _outputs)
			l->close();
	}

	void Logger::addOutput(LogOutputBase * output) {
		_outputs.add(output);
	}

	void Logger::clear() {
		for (LogOutputBase* l : _outputs)
			l->clear();
	}

	void Logger::write(const FeString & msg, const Color & color) {
		for (LogOutputBase* l : _outputs)
			l->write(msg, color);
	}

	void Logger::writeLine(const FeString & msg, const Color & color) {
		FeString result = "["_fe + FeString::dateTime(U"%X") + "] "_fe + msg; // TODO do we need a proper FeString-builder/stream for situations like this?
		for (LogOutputBase* l : _outputs)
			l->writeLine(result, color);
	}
}