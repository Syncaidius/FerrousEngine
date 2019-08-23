#pragma once
#include <glm.hpp>

// Forward declations for all shapes.
namespace fe {
	namespace shapes {
		template<typename T, glm::qualifier Q>
		struct Rect;

		template<typename T, glm::qualifier Q>
		struct Circle;

		template<typename T, glm::qualifier Q>
		struct Line;

		template<typename T, glm::qualifier Q>
		struct Triangle;
	}
}