#pragma once
#include "stdafx.h"
#include <locale>

namespace fe {
	class Localization {
	public:
		static Localization* get() { return _localization; }

		bool setCurrentLocale(wchar_t* locale_name);
		bool setCurrentLocale(std::locale* locale);

		std::locale* getCurrentLocale() { return _loc_current; }
		std::locale* getPreferredLocale() { return _loc_preferred; }
		std::locale* getDefaultLocale() { return _loc_default; }

	private:
		Localization();
		~Localization();

		static Localization* _localization;
		std::locale* _loc_current;
		std::locale* _loc_preferred;
		std::locale* _loc_default;
	};
}