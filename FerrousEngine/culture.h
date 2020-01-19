#pragma once
#include "strings.h"
#include "allocation.h"
#include "dictionary.h"

using namespace fe::collections;

namespace fe {
	enum class TextFlowDirection {
		LeftToRight = 0,
		RightToLeft = 1
	};

	struct NumberFormatInfo {
		char32_t CurrencySymbol;
		char32_t NumberSeparator;
		char32_t DecimalPoint;
		char32_t NegativeSign;
		char32_t PositiveSign;
		char32_t MultiplySign;
		char32_t PercentSymbol;
		char32_t EqualSymbol;

		/* The grouping size of digits in large numbers. Most western cultures use a value of 3. */
		uint8_t GroupSize;

		bool operator ==(const NumberFormatInfo& other) const {
			// TODO optimize this.
			return CurrencySymbol == other.CurrencySymbol &&
				DecimalPoint == other.DecimalPoint &&
				EqualSymbol == other.EqualSymbol &&
				GroupSize == other.GroupSize &&
				MultiplySign == other.MultiplySign &&
				NegativeSign == other.NegativeSign &&
				NumberSeparator == other.NumberSeparator &&
				PercentSymbol == other.PercentSymbol &&
				PositiveSign == other.PositiveSign;
		}
	};

	struct TextFormatInfo {
		char32_t ListSeparator;
		TextFlowDirection Direction;
		char32_t (*toLower)(const char32_t&);
		char32_t (*toUpper)(const char32_t&);
		bool (*isSpace)(const char32_t&);
		bool (*isWhitespace)(const char32_t&);

		bool operator ==(const TextFormatInfo& other) const {
			// TODO optimize this.
			return Direction == other.Direction &&
				isSpace == other.isSpace &&
				ListSeparator == other.ListSeparator &&
				toLower == other.toLower &&
				toUpper == other.toUpper;
		}

	};

	struct CultureInfo { 
	public:
		/* Default constructor, which builds defaults to invariant-culture values.*/
		CultureInfo();

		FeString DisplayName;

		FeString EnglishName;

		/*	The ISO 3166-1 country code of the current culture.
			See: https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes */
		FeString Iso3166;

		/*	The ISO 639-1 language code of the current culture.
			See: https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes */
		FeString Iso639;

		NumberFormatInfo NumberFormat;
		TextFormatInfo TextFormat;

		bool operator ==(const CultureInfo& other) const {
			// TODO optimize this.
			return DisplayName == other.DisplayName &&
				EnglishName == other.EnglishName &&
				Iso3166 == other.Iso3166 &&
				Iso639 == other.Iso639 &&
				NumberFormat == other.NumberFormat &&
				TextFormat == other.TextFormat;
		}

	private:
		static char32_t defaultLatinToLower(const char32_t& c);
		static char32_t defaultLatinToUpper(const char32_t& c);
		static bool defaultLatinIsSpace(const char32_t& c);
		static bool defaultLatinIsWhitespace(const char32_t& c);
	};

	class Culture {
	public:
		const FeString& displayName() const;

		const FeString& englishName() const;

		const FeString& iso3166CountryCode() const;

		const FeString& iso639LanguageCode() const;

		const FeString& isoName() const;

		char32_t toLower(const char32_t& character) const;

		char32_t toUpper(const char32_t& character) const;

		bool isSpace(const char32_t& character) const;

		bool isWhitespace(const char32_t& character) const;

		const NumberFormatInfo& numberInfo() const;

		/* The culture's text flow direction. */
		TextFlowDirection textDirection() const;

		/* The character used when separating items in a list. */
		char32_t listSeparator() const;

		inline const FeString& operator[] (const FeString& key) {
			return _strings[key];
		}

		bool operator ==(const Culture& other) const {
			return _info == other._info;
		}

		bool operator !=(const Culture& other) const {
			return !(_info == other._info);
		}

	private:
		friend class Localization;

		Culture();
		Culture(const CultureInfo& info, FerrousAllocator* allocator = Memory::get());

		const CultureInfo _info;
		Dictionary<FeString, FeString> _strings;
	};
}