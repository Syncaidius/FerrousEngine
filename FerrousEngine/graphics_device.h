#pragma once
#include "strings.h"

namespace fe {
	class GraphicsDevice {
	public:
		virtual FeString getName() = 0;
		virtual uint64_t getMaxVRAM() = 0;
		virtual uint64_t getCurrentVRAM() = 0;
	};
}