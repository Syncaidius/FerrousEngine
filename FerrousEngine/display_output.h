#pragma once
#include "strings.h"
#include "rect.h"

class DisplayOutput {
public:
	enum class Orientation {
		// Unspecified rotation
		Unspecified = 0,

		// Specifies no rotation
		Identity = 1,

		// Specifies 90 degrees of rotation
		Rotate90 = 2,

		// Specifies 180 degrees of rotation
		Rotate180 = 3,

		// Specifies 270 degrees of rotation
		Rotate270 = 4
	};

	virtual FeString getName() = 0;
	virtual Orientation getOrientation() = 0;
	virtual Rect32 getBounds() = 0;
};