#include "logging_console.h";
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
		SetConsoleCP(65001);
		SetConsoleOutputCP(65001);
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
		UtfString utf8 = UtfString(msg, fe::UtfEncoding::UTF8);
		SetConsoleTextAttribute(_console_handle, getColorFlags(color));
		std::cout << utf8.getData();
	}

	void ConsoleLogOutput::writeLine(const FeString& msg, const Color& color) {
		UtfString utf8 = UtfString(msg, fe::UtfEncoding::UTF8);
		SetConsoleTextAttribute(_console_handle, getColorFlags(color));
		std::cout << utf8.getData() << std::endl;
	}

	void ConsoleLogOutput::clear() {
		// See for other OS clear implementations: http://www.cplusplus.com/articles/4z18T05o/

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		DWORD                      count;
		DWORD                      cellCount;
		COORD                      homeCoords = { 0, 0 };

		/* Get the number of cells in the current buffer */
		if (!GetConsoleScreenBufferInfo(_console_handle, &csbi))
			return;

		cellCount = csbi.dwSize.X * csbi.dwSize.Y;

		/* Fill the entire buffer with spaces */
		if (!FillConsoleOutputCharacter(
			_console_handle,
			L' ',
			cellCount,
			homeCoords,
			&count
		)) return;

		/* Fill the entire buffer with the current colors and attributes */
		if (!FillConsoleOutputAttribute(
			_console_handle,
			csbi.wAttributes,
			cellCount,
			homeCoords,
			&count
		)) return;

		/* Move the cursor home */
		SetConsoleCursorPosition(_console_handle, homeCoords);
	}

	void ConsoleLogOutput::close() {

	}
}