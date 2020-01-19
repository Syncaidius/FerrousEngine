#pragma once
#include "stdafx.h"
#include "strings.h";
#include "logging.h";

using namespace fe;

class FerrousTest {
public:
	FerrousTest(const FeString& testTitle) : title(testTitle) 
	{
	}

	void run(Logger& log) {
		log.writeLine("Press any key to run " + title + "..."_fe, Color::yellow);
		std::cin.get();

		log.writeLine(title, Color::limeGreen);
		log.writeLine(FeString::repeat("=", 20 + title.len()), Color::limeGreen);
		onRun(log);
		log.writeLine(" ");
	}

	const FeString title;

protected:
	virtual void onRun(Logger& log) = 0;
};