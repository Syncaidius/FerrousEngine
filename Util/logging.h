#pragma once
#include "stdafx.h"
#include "memory.h"
#include "string_fe.h"

class FERROUS_UTIL_API LogOutputBase {
public:
	virtual void write(const FeString& msg) = 0;
	virtual void writeLine(const FeString& msg) = 0;
	virtual void clear() = 0;
	virtual void close() = 0;
};

class FERROUS_UTIL_API LogConsoleOutput : public LogOutputBase {
public:
	LogConsoleOutput();
	~LogConsoleOutput();

protected:
	friend class Logger;

	void write(const FeString& msg);
	void writeLine(const FeString& msg);
	void clear();
	void close();
private:
	void redirectToConsole(void);
	void* _console_handle;
};

class FERROUS_UTIL_API Logger {
public:
	struct LogOutputHandle {
		LogOutputBase* output;
	};
	Logger(uint16_t initial_slot_count = 1);
	~Logger();

	void addOutput(LogOutputBase* output);

	void writeLine(const FeString& msg);

	void write(const FeString& msg);

	void clear();

private:
	LogOutputHandle* _outputs;
	uint16_t _output_slot_count;
};