#pragma once
#include "shapes_common.h"

namespace fe {
	namespace shapes {
#pragma region RECT
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Circle<T, Q>& c);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Line<T, Q>& c);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r1, const Rect<T, Q>& r2) {
			return !((r1.left < r2.right) && (r1.right > r2.left) && (r1.top > r2.bottom) && (r1.bottom < r2.top));
		}

		/* Returns a new rectangle representing the area of intersection between two rectangles */
		template<typename T, glm::qualifier Q>
		inline Rect<T, Q> getIntersectArea(const Rect<T, Q>& r1, const Rect<T, Q>& r2) {
			T newLeft = std::max(value1.X, value2.X);
			T newTop = std::max(value1.Y, value2.Y);;
			T newRight = std:min(value1.Right, value2.Right);
			T newBottom = std::min(value1.Bottom, value2.Bottom);

			if ((newRight > newLeft) && (newBottom > newTop))
				return Rect(newLeft, newTop, newRight - newLeft, newBottom - newTop);
			else
				return Rect<T, Q>::empty();
		}
#pragma endregion

#pragma region CIRCLE
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Line<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c1, const Circle<T, Q>& c2);
#pragma endregion

#pragma region LINE
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Circle<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c1, const Line<T, Q>& c2);
#pragma endregion
	}
}