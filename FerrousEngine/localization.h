#pragma once
#include <stdint.h>
#include "culture.h"
#include "allocation.h"
#include "dictionary.h"

namespace fe {
	class Localization {
	public:
		static Localization* get();
		static void initialize();

		/* Attempts to register and return a new culture. 
		If another culture with the same name already exists, that one will be returned instead. */
		Culture* registerCulture(const CultureInfo& cultureInfo);

		bool unregisterCulture(const FeString& name);

		bool hasCulture(const FeString& name);

		bool setCurrentCulture(const FeString& cultureName);

		Culture* getCurrentCulture();

		bool setDefaultCulture(const FeString& cultureName);

		Culture* getDefaultCulture();

		const Culture* getCulture(const FeString& name);

	private:
		static Localization* _instance;

		Localization(const CultureInfo& defaultCulture, FerrousAllocator* allocator);

		Dictionary<FeString, Culture*>* _cultures;
		
		FerrousAllocator* _allocator;
		Culture* _currentCulture;
		Culture* _defaultCulture;
	};
}