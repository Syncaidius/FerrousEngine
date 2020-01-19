#include "culture.h"

namespace fe {
	CultureInfo::CultureInfo() {
		DisplayName = U"Invariant Culture";
		EnglishName = U"Invariant Culture";
		Iso3166 = U"iv";
		Iso639 = U"iv";

		NumberFormat = NumberFormatInfo();
		NumberFormat.CurrencySymbol = U'§';
		NumberFormat.NumberSeparator = U',';
		NumberFormat.DecimalPoint = U'.';
		NumberFormat.NegativeSign = U'-';
		NumberFormat.PositiveSign = U'+';
		NumberFormat.PercentSymbol = U'%';
		NumberFormat.EqualSymbol = U'=';
		TextFormat.ListSeparator = ',';
		TextFormat.Direction = TextFlowDirection::LeftToRight;
		TextFormat.toLower = &defaultLatinToLower;
		TextFormat.toUpper = &defaultLatinToUpper;
		TextFormat.isSpace = &defaultLatinIsSpace;
		TextFormat.isWhitespace = &defaultLatinIsWhitespace;
	}

	char32_t CultureInfo::defaultLatinToLower(const char32_t& c) {
		if (c >= 65 && c <= 90)
			return c + 32;

		return c;
	}

	char32_t CultureInfo::defaultLatinToUpper(const char32_t& c) {
		if (c >= 97 && c <= 122)
			return c - 32;

		return c;
	}

	bool CultureInfo::defaultLatinIsSpace(const char32_t& c) {
		return c == 32;
	}

	bool CultureInfo::defaultLatinIsWhitespace(const char32_t& c) {
		return c == U' ' || c==U'\t' || c==U'\n' || c==U'\r';
	}

	Culture::Culture() : _info() {
		_strings = Dictionary<FeString, FeString>();
	}

	Culture::Culture(const CultureInfo& info, FerrousAllocator* allocator) : _info(info) {
		_strings = Dictionary<FeString, FeString>();
	}

	const FeString& Culture::displayName() const {
		return _info.DisplayName;
	}

	const FeString& Culture::englishName() const {
		return _info.EnglishName;
	}

	char32_t Culture::toLower(const char32_t& character) const {
		return _info.TextFormat.toLower(character);
	}

	char32_t Culture::toUpper(const char32_t& character) const {
		return _info.TextFormat.toUpper(character);
	}

	bool Culture::isSpace(const char32_t& character) const {
		return _info.TextFormat.isSpace(character);
	}

	bool Culture::isWhitespace(const char32_t& character) const {
		return _info.TextFormat.isWhitespace(character);
	}

	const NumberFormatInfo& Culture::numberInfo() const {
		return _info.NumberFormat;
	}

	TextFlowDirection Culture::textDirection() const {
		return _info.TextFormat.Direction;
	}

	char32_t Culture::listSeparator() const {
		return _info.TextFormat.ListSeparator;
	}

	const FeString& Culture::iso3166CountryCode() const {
		return _info.Iso3166;
	}

	const FeString& Culture::iso639LanguageCode() const {
		return _info.Iso639;
	}

	const FeString& Culture::isoName() const {
		return _info.Iso639 + U"-"_fe + _info.Iso3166;
	}
}