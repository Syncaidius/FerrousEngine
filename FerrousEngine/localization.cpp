#include "localization.h"

namespace fe {
	Localization* Localization::_localization = new Localization();

	Localization::Localization() {
		_loc_preferred = new std::locale(""); /* Get user's preferred locale.*/
		_loc_default = new std::locale("C");
		_loc_current = _loc_default;
	}

	Localization::~Localization() {
		if (_loc_current != nullptr &&
			_loc_current != _loc_preferred &&
			_loc_current != _loc_default) {
			delete _loc_current;
		}

		delete _loc_current;
		delete _loc_preferred;
	}

	bool Localization::setCurrentLocale(std::locale* locale) {
		if (locale != nullptr)
			_loc_current = locale;
		else
			_loc_current = _loc_default;

		return true;
	}
}