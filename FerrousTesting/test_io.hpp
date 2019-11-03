#pragma once
#include "ferrous_test.hpp"
#include "file.h"
#include "stream_file.h"
#include "stream_text_reader.h";
#include "stream_text_writer.h";

class TestIO : public FerrousTest {
public:

	TestIO() : FerrousTest("I/O Test") {}

protected:
	void onRun(Logger& log) override {
		log.writeLine("Press any key to clear log.", Color::yellow);
		cin.get();

		log.clear();

		FeString workingDir = File::getWorkingDirectory();
		log.writeLineF("Current Path: %s"_fe, workingDir.getData());
		log.writeLineF("   Is directory: %d"_fe, File::isDirectory(workingDir));
		log.writeLineF("   Is file: %d"_fe, File::isFile(&workingDir));

		bool fExists = File::exists(L"FerrousTesting.exe");
		log.writeLineF("File \"FerrousTesting.exe\" found: %d"_fe, fExists);

		FileStream testFileOut = FileStream("test_file.txt"_fe, FileStreamFlags::Create, false, true);
		TextStreamWriter testWriter = TextStreamWriter(&testFileOut, Memory::get());
		log.writeLineF("File created successfully!"_fe);
		testWriter.writeLine(U"This is a test file! 这是一个测试文件! Это тестовый файл! これはテストファイルです!"_fe);

		size_t fSize = testFileOut.getSize();
		testFileOut.close();

		log.writeLineF("Written %d bytes to file"_fe, fSize);
		log.writeLine("File closed"_fe, Color::green);

		// Now open to read
		FileStream testFileIn = FileStream("test_file.txt"_fe, FileStreamFlags::None, true, false);
		TextStreamReader testReader = TextStreamReader(&testFileIn, Memory::get());

		FeString stringFromFile = L"\0"_fe;
		fSize = testFileIn.getSize();

		log.writeLineF("Open file with size: %d bytes"_fe, Color::limeGreen, fSize);
		testReader.readLine(&stringFromFile);
		testFileIn.close();

		log.writeLine("File closed"_fe, Color::red);
		log.writeLineF("String read from file: %s"_fe, Color::darkRed, stringFromFile.getData());

		if (File::exists("UTF-8-demo.txt")) {
			FileStream utf8File = FileStream("UTF-8-demo.txt", FileStreamFlags::Binary, true, false);
			TextStreamReader utf8Reader = TextStreamReader(&utf8File, Memory::get());

			size_t utf8FileSize = utf8File.getSize();
			while (utf8File.getReadPos() < utf8FileSize) {
				FeString line;
				utf8Reader.readLine(&line);
				log.writeLine(line);
			}

			utf8File.close();
		}
		else {
			log.writeLine("UTF-8 test file is missing.");
		}
	}
};