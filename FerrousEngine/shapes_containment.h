#pragma once
#include "shapes_common.h"

namespace fe {
	namespace shapes {
#pragma region RECT
		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const Circle<T, Q>& c);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const Line<T, Q>& c);

		/* Returns true if the first rectangle (r1) contains the second rectangle (r2). */
		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r1, const Rect<T, Q>& r2) {
			return (r2.left >= r1.left) && (r2.right <= r1.right) && (r2.top >= r1.top) && (r2.bottom <= r.bottom);
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const T x, const T y) {
			return (x >= r.left) && (x <= r.right) && (y >= r.top) && (y <= r.bottom);
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const glm::vec<2, T, Q> v) {
			return (v.x >= r.left) && (v.x <= r.right) && (v.y >= r.top) && (v.y <= r.bottom);
		}
#pragma endregion

#pragma region CIRCLE
		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Line<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c1, const Circle<T, Q>& c2);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const T x, const T y) {
			return glm::distance(c.origin, glm::vec<2, T, Q>(x, y)) <= c.radius;
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& r, const glm::vec<2, T, Q> v) {
			return glm::distance(c.origin, v) <= c.radius;
		}
#pragma endregion
	}
}