#include "logging.h";
#include <Windows.h>
#include <iostream>

LogConsoleOutput::LogConsoleOutput() {
	redirectToConsole();
	_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

LogConsoleOutput::~LogConsoleOutput() {
	FreeConsole();
}

void LogConsoleOutput::write(const FeString& msg) {
	std::wcout << msg.getData();
}

void LogConsoleOutput::writeLine(const FeString& msg) {
	std::wcout << msg.getData() << std::endl;
}

void LogConsoleOutput::clear() {

}

void LogConsoleOutput::close() {

}

void LogConsoleOutput::redirectToConsole() {
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE* fp;

	FILE* pNewStdout = nullptr;
	FILE* pNewStderr = nullptr;
	FILE* pNewStdin = nullptr;

	// allocate a console for this app
	AllocConsole();
	freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
	freopen_s(&pNewStdin, "CONIN$", "r", stdin);

	// Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
	// to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
	// which seems to occur in newer version of Visual Studio even when the console has not been read from or written
	// to yet.
	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();

	std::wcout.clear();
	std::wcerr.clear();
	std::wcin.clear();
}