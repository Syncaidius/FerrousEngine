#pragma once
#include <stdint.h>
#include <glm.hpp>

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	Color();
	Color(uint8_t rv, uint8_t gv, uint8_t bv, uint8_t av);

	static void toVec4(const Color& c, glm::vec4& v);
	static void fromVec4(const glm::vec4& v, Color& c);
	static void fromPacked(uint32_t packed, Color& c);

	const static Color white;
	const static Color black;
	const static Color darkRed;
	const static Color red;
	const static Color green;
	const static Color limeGreen;
	const static Color darkBlue;
	const static Color blue;
	const static Color yellow;
	const static Color orange;
	const static Color purple;
	const static Color cornflower_blue;
	const static Color grey;
	const static Color pink;
};