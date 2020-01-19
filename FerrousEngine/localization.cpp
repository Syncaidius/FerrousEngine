#include "localization.h"

namespace fe {
	Localization* Localization::_instance = nullptr;

	Localization* Localization::get() {
		return _instance;
	}

	void Localization::initialize() {
		CultureInfo info = CultureInfo();
		info.DisplayName = "English US";
		info.EnglishName = "English US";
		info.Iso639 = "en";
		info.Iso3166 = "us";
		info.NumberFormat.CurrencySymbol = '$';
		info.NumberFormat.DecimalPoint = '.';
		info.NumberFormat.EqualSymbol = '=';
		info.NumberFormat.GroupSize = 3;
		info.NumberFormat.NegativeSign = '-';
		info.NumberFormat.NumberSeparator = ',';
		info.NumberFormat.PercentSymbol = '%';
		info.NumberFormat.PositiveSign = '+';
		info.TextFormat.ListSeparator = ',';
		info.TextFormat.Direction = TextFlowDirection::LeftToRight;

		if (_instance == nullptr) {
			Memory* allocator = Memory::get();
			_instance = allocator->allocType<Localization>();
			new (_instance) Localization(info, allocator);
		}
		else {
			Culture* culture = _instance->registerCulture(info);
			_instance->_defaultCulture = culture;
			_instance->_currentCulture = culture;
		}
	}

	Localization::Localization(const CultureInfo& defaultCulture, FerrousAllocator* allocator) {
		_allocator = allocator;
		_cultures = _allocator->allocType<Dictionary<FeString, Culture*>>();
		new (_cultures) Dictionary<FeString, Culture*>(4, _allocator);

		_defaultCulture = registerCulture(defaultCulture);
		_currentCulture = _defaultCulture;
	}

	Culture* Localization::registerCulture(const CultureInfo& info) {
		if (_cultures->containsKey(info.EnglishName)) {
			return (*_cultures)[info.EnglishName];
		}
		else {
			Culture* culture = _allocator->allocType<Culture>();
			new (culture) Culture(info);
			_cultures->put(info.EnglishName, culture);
			return culture;
		}
	}

	bool Localization::unregisterCulture(const FeString& name) {
		return _cultures->remove(name);
	}

	bool Localization::hasCulture(const FeString& name) {
		return _cultures->containsKey(name);
	}

	bool Localization::setCurrentCulture(const FeString& name) {
		Culture* culture = nullptr;
		if (_cultures->tryGet(name, culture)) {
			_currentCulture = culture;
			return true;
		}

		return false;
	}

	Culture* Localization::getCurrentCulture() {
		return _currentCulture;
	}

	bool Localization::setDefaultCulture(const FeString& name) {
		Culture* culture = nullptr;
		if (_cultures->tryGet(name, culture)) {
			_defaultCulture = culture;
			return true;
		}

		return false;
	}

	Culture* Localization::getDefaultCulture() {
		return _defaultCulture;
	}

	const Culture* Localization::getCulture(const FeString& name) {
		return _cultures->get(name);
	}
}