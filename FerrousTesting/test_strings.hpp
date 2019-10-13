#pragma once
#include "stdafx.h"
#include "ferrous_test.hpp"
#include "localization.h"

using namespace std;
class TestStrings : public FerrousTest {
public:

	TestStrings() : FerrousTest("String Test") {}

protected:
	void onRun(Logger& log) override {
		Localization* loc = Localization::get();
		loc->setCurrentLocale(loc->getPreferredLocale());
		log.writeLine(L"FeString Test"_fe);
		log.writeLine(L"============="_fe);

		FeString aString("String A"_fe);
		FeString bString(L"String B"_fe);
		FeString cString = " = String C"_fe;
		FeString formattedString = FeString::format("%d plus %d equals %d"_fe, 5, 6, 5 + 6);
		//FeString result = aString + 1 + " + "_fe + bString + 2.3458 + " = The result"_fe;
		//FeString resultLower = result.toLower();
		//FeString resultUpper = result.toUpper();
		FeString resultCapsAll = ("this is a capitalized string"_fe).capitalize();
		FeString resultCapsFirst = ("this is a capitalized string"_fe).capitalizeFirst();

		FeString toTrim = "			   I am trimmed		"_fe;
		FeString resultTrimmed = toTrim.trim();
		FeString resultTrimStart = toTrim.trimStart();
		FeString resultTrimEnd = toTrim.trimEnd();

		FeString toSearch = "I am a string. I can be searched.";
		size_t indexOfString = toSearch.indexOf(&"string"_fe);
		size_t indexOfSearched = toSearch.indexOf(&L"searched"_fe);
		size_t indexOfChicken = toSearch.indexOf(&"chicken"_fe);

		FeString toReplace = "I am a replaced string full of replacements, which replaces things.";
		FeString resultReplaced = toReplace.replace("replace", "start");
		FeString resultSubStr = toReplace.substr(0, 10);

		bool startsWith = toSearch.startsWith(&"I am"_fe);
		bool startsWithFail = toSearch.startsWith(&"I'm not"_fe);
		bool endsWith = toSearch.endsWith(&"searched."_fe);
		bool endsWithFail = toSearch.endsWith(&"searching!"_fe);

		log.writeLineF("A string: %s", aString.getData());
		log.writeLineF("B string:  %s", bString.getData());
		log.writeLineF("C string:  %s", cString.getData());
		log.writeLineF("Formatted:  %s", formattedString.getData());
		//log.writeLineF("Result:  %s", result.getData());
		log.writeLine(" ");

		//log.writeLineF("Lower-case: %s", resultLower.getData());
		//log.writeLineF("Upper-case: %s",resultUpper.getData());
		log.writeLineF("Capitalized (all): %s", resultCapsAll.getData());
		log.writeLineF("Capitalized (1st): %s", resultCapsFirst.getData());
		log.writeLine(" ");

		log.writeLineF("Trimmed: {%s}", resultTrimmed.getData());
		log.writeLineF("Trimmed (start): {%s}", resultTrimStart.getData());
		log.writeLineF("Trimmed (end): {%s}", resultTrimEnd.getData());
		log.writeLine(" ");

		log.writeLineF("Replacement Target: %s", toReplace.getData());
		log.writeLineF("Replace \"replace\" with \"start\": {%s}", resultReplaced.getData());
		log.writeLineF("Substr 0 to 10: {%s}", resultSubStr.getData());
		log.writeLine(" ");

		log.writeLineF("Target: %s", toSearch.getData());
		log.writeLineF("indexOf(\"string\"): %d", indexOfString);
		log.writeLineF("indexOf(\"searched\"): %d", indexOfSearched);

		log.writeLineF("indexOf(\"chicken\"): %s", (indexOfChicken == FeString::INDEXOF_NONE ? L"not found" : L"found"));
		log.writeLineF("startsWith(\"I am\"): %s", (startsWith ? L"true" : L"false"));
		log.writeLineF("startsWith(\"I'm not\"): %s", (startsWithFail ? L"true" : L"false"));
		log.writeLineF("endsWith(\"searched.\"): %s", (endsWith ? L"true" : L"false"));
		log.writeLineF("endsWith(\"searching!\"): %s", endsWithFail ? L"true" : L"false");
		log.writeLine(L" "_fe);

		log.writeLine(L" "_fe); // TODO empty line overload.
		/*log.writeLine(L"MEMORY AFTER STRING TEST");*/
		Memory::get()->outputDebug();

		uint32_t defrag_iterations = 1;
		Memory::get()->defragment(1);
		cout << endl;
		cout << "AFTER MEM DEFRAGMENTATION" << endl;
		Memory::get()->outputDebug();
	}
};