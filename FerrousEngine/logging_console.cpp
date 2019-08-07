#include "logging.h";
#include <Windows.h>
#include <iostream>

namespace fe {
	ConsoleLogOutput::ConsoleLogOutput() {
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
		_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	ConsoleLogOutput::~ConsoleLogOutput() {
		FreeConsole();
	}

	WORD ConsoleLogOutput::getColorFlags(const Color& color) {
		int colFlags = 0;
		if (color.r > 0)
		{
			colFlags |= FOREGROUND_RED;
			if (color.r > 127)
				colFlags |= FOREGROUND_INTENSITY;
		}

		if (color.g > 0)
		{
			colFlags |= FOREGROUND_GREEN;
			if (color.g > 127)
				colFlags |= FOREGROUND_INTENSITY;
		}

		if (color.b > 0)
		{
			colFlags |= FOREGROUND_BLUE;
			if (color.b > 127)
				colFlags |= FOREGROUND_INTENSITY;
		}

		return colFlags;
	}

	void ConsoleLogOutput::write(const FeString& msg, const Color& color) {
		SetConsoleTextAttribute(_console_handle, getColorFlags(color));
		std::wcout << msg.getData();
	}

	void ConsoleLogOutput::writeLine(const FeString& msg, const Color& color) {
		SetConsoleTextAttribute(_console_handle, getColorFlags(color));
		std::wcout << msg.getData() << std::endl;
	}

	void ConsoleLogOutput::clear() {

	}

	void ConsoleLogOutput::close() {

	}
}