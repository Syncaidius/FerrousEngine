#pragma once
#include "stdafx.h"
#include "memory_allocator.h"

class FERROUS_UTIL_API LogOutputBase {
public:
	virtual void write(const wchar_t* msg) = 0;
	virtual void writeLine(const wchar_t* msg) = 0;

	virtual void clear() = 0;

	virtual void close() = 0;
};

class FERROUS_UTIL_API LogConsoleOutput : public LogOutputBase {
public:
	LogConsoleOutput();
	~LogConsoleOutput();

	void write(const wchar_t* msg);
	void writeLine(const wchar_t* msg);
	void clear();
	void close();
private:
	void RedirectToConsole(void);
	void* m_console_handle;
};

class FERROUS_UTIL_API Logger {
public:
	struct LogOutputHandle {
		LogOutputBase* output;
	};
	Logger(MemoryAllocator* alloc, uint16_t initial_slot_count = 1);
	~Logger();

	void addOutput(LogOutputBase* output);

	void writeLine(const wchar_t* msg);

	void write(const wchar_t* msg);

	void clear();

private:
	LogOutputHandle* m_outputs;
	MemoryAllocator* m_allocator;
	uint16_t m_output_slot_count;
};