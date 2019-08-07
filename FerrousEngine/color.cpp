#include "color.h"

namespace fe {
	const Color Color::white(255, 255, 255, 255);
	const Color Color::black(0, 0, 0, 255);
	const Color Color::grey(127, 127, 127, 255);
	const Color Color::darkRed(127, 0, 0, 255);
	const Color Color::red(255, 0, 0, 255);
	const Color Color::green(0, 127, 0, 255);
	const Color Color::limeGreen(0, 255, 0, 255);
	const Color Color::darkBlue(0, 0, 127, 255);
	const Color Color::blue(0, 0, 255, 255);
	const Color Color::yellow(255, 255, 0, 255);
	const Color Color::orange(255, 165, 0, 255);
	const Color Color::purple(128, 0, 128, 255);
	const Color Color::cornflower_blue(100, 149, 237, 255);
	const Color Color::pink(255, 192, 203, 255);

	Color::Color(uint8_t rv, uint8_t gv, uint8_t bv, uint8_t av) {
		r = rv;
		g = gv;
		b = bv;
		a = av;
	}

	void Color::toVec4(const Color& c, glm::vec4& v) {
		v.r = static_cast<uint8_t>(c.r / 255.0f);
		v.g = static_cast<uint8_t>(c.g / 255.0f);
		v.b = static_cast<uint8_t>(c.b / 255.0f);
		v.a = static_cast<uint8_t>(c.a / 255.0f);
	}

	void Color::fromVec4(const glm::vec4 & v, Color & c) {
		c.r = 255.0f * v.r;
		c.g = 255.0f * v.g;
		c.b = 255.0f * v.b;
		c.a = 255.0f * v.a;
	}

	void Color::fromPacked(uint32_t packed, Color & c) {
		c.r = (packed >> 24) & 0xFF;
		c.g = (packed >> 16) & 0xFF;
		c.b = (packed >> 8) & 0xFF;
		c.a = packed & 0xFF;
	}
}