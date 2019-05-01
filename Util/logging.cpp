#include "logging.h"
#include <time.h>
#include <stdlib.h>

Logger::Logger(MemoryAllocator* alloc, uint16_t initial_slot_count) {
	m_allocator = alloc;
	m_output_slot_count = initial_slot_count;
	m_outputs = m_allocator->allocArray<LogOutputHandle>(m_output_slot_count);
}

Logger::~Logger() {
	m_allocator->release(m_outputs);
}

void Logger::addOutput(LogOutputBase* output) {
	// Try and find an existing output slot before resizing the array.
	LogOutputHandle* end = m_outputs;
	end += m_output_slot_count;

	LogOutputHandle* i = nullptr;
	for (i = m_outputs; i != end; i++) {
		if (i->output == nullptr) {
			i->output = output;
			return;
		}
	}

	// If we've reached here, no slots were found. Expand!
	uint64_t next = m_output_slot_count;
	m_allocator->resizeArray<LogOutputHandle>(m_outputs, m_output_slot_count, ++m_output_slot_count);

	// Jump straight to the slot we just added.
	i = m_outputs;
	i += next;
	*i->output = *output;
}

void Logger::clear() {
	LogOutputHandle* end = m_outputs;
	end += m_output_slot_count;
	for (LogOutputHandle* i = m_outputs; i != end; i++) {
		if (i != nullptr)
			i->output->clear();
	}
}

void Logger::write(const wchar_t* msg) {
	LogOutputHandle* end = m_outputs;
	end += m_output_slot_count;
	for (LogOutputHandle* i = m_outputs; i != end; i++) {
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
	wchar_t* cc = m_allocator->allocArray<wchar_t>(strlen);
	wcscat_s(cc, strlen, L"[");
	wcscat_s(cc, strlen, buf);
	wcscat_s(cc, strlen, L"] ");
	wcscat_s(cc, strlen, msg);

	LogOutputHandle* end = m_outputs;
	end += m_output_slot_count;
	for (LogOutputHandle* i = m_outputs; i != end; i++) {
		if (i != nullptr)
			i->output->writeLine(cc);
	}

	// TODO implement stream-based writing
	// See: https://stackoverflow.com/questions/14086417/how-to-write-custom-input-stream-in-c
}