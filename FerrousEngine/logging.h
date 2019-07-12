#pragma once
#include "stdafx.h"
#include "allocation.h"
#include "strings.h"

class LogOutputBase {
public:
	virtual void write(const FeString& msg) = 0;
	virtual void writeLine(const FeString& msg) = 0;
	virtual void clear() = 0;
	virtual void close() = 0;
};

class LogConsoleOutput : public LogOutputBase {
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

class Logger {
public:
	struct LogOutputHandle {
		LogOutputBase* output;
	};
	Logger(uint16_t initial_slot_count = 1);
	~Logger();

	void addOutput(LogOutputBase* output);

	void writeLine(const FeString& msg);

	void write(const FeString& msg);

	/* A formattable version of write() */
	void writeF(const FeString& str, ...);

	/* A formattable version of writeLine()*/
	void writeLineF(const FeString& str, ...);

	void clear();

private:
	LogOutputHandle* _outputs;
	uint16_t _output_slot_count;
};