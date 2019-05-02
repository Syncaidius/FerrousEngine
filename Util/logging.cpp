#include "logging.h"
#include <time.h>
#include <stdlib.h>

Logger::Logger(uint16_t initial_slot_count) {
	_output_slot_count = initial_slot_count;
	_outputs = MemoryAllocator::get()->allocArray<LogOutputHandle>(_output_slot_count);
}

Logger::~Logger() {
	LogOutputHandle* end = _outputs;
	end += _output_slot_count;
	for (LogOutputHandle* i = _outputs; i != end; i++) {
		if (i != nullptr)
			i->output->close();
	}
}

void Logger::addOutput(LogOutputBase* output) {
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
	uint64_t next = _output_slot_count++;
	LogOutputHandle* replacement = new LogOutputHandle[_output_slot_count];
	memcpy(replacement, _outputs, sizeof(LogOutputHandle) * next);
	delete [] _outputs;
	_outputs = replacement;

	// Jump straight to the slot we just added.
	i = _outputs;
	i += next;
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

void Logger::write(const wchar_t* msg) {
	LogOutputHandle* end = _outputs;
	end += _output_slot_count;
	for (LogOutputHandle* i = _outputs; i != end; i++) {
		if (i != nullptr)
			i->output->write(msg);
	}
}

void Logger::writeLine(const wchar_t* msg) {
	// Timestamp
	time_t now = time(0);
	struct tm tstruct;
	wchar_t buf[80];
	localtime_s(&tstruct , &now);
	wcsftime(buf, sizeof(buf), L"%X", &tstruct);

	const size_t msglen = wcslen(msg);
	const size_t strlen = msglen + wcslen(buf) + 4; // 4 extra chars for the [], space and null termination.
	wchar_t* cc = MemoryAllocator::get()->allocArray<wchar_t>(strlen);
	wcscat_s(cc, strlen, L"[");
	wcscat_s(cc, strlen, buf);
	wcscat_s(cc, strlen, L"] ");
	wcscat_s(cc, strlen, msg);

	LogOutputHandle* end = _outputs;
	end += _output_slot_count;
	for (LogOutputHandle* i = _outputs; i != end; i++) {
		if (i != nullptr)
			i->output->writeLine(cc);
	}

	// TODO implement stream-based writing
	// See: https://stackoverflow.com/questions/14086417/how-to-write-custom-input-stream-in-c
}