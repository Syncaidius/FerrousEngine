#pragma once
#include "shapes_common.h"

namespace fe {
	namespace shapes {
#pragma region RECT
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Circle<T, Q>& c);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Line<T, Q>& l);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r1, const Rect<T, Q>& r2) {
			return !((r1.left < r2.right) && (r1.right > r2.left) && (r1.top > r2.bottom) && (r1.bottom < r2.top));
		}

		/* Returns a new rectangle representing the area of intersection between two rectangles */
		template<typename T, glm::qualifier Q>
		inline Rect<T, Q> getIntersectArea(const Rect<T, Q>& r1, const Rect<T, Q>& r2) {
			T newLeft = std::max(r1.X, r2.X);
			T newTop = std::max(r1.Y, r2.Y);
			T newRight = std::min(r1.Right, r2.Right);
			T newBottom = std::min(r1.Bottom, r2.Bottom);

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
		inline bool intersects(const Circle<T, Q>& c, const Line<T, Q>& l);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c1, const Circle<T, Q>& c2);
#pragma endregion

#pragma region LINE
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Circle<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& l1, const Line<T, Q>& l2) {
			// Line AB represented as a1x + b1y = c1 
			double a1 = B.second - A.second;
			double b1 = A.first - B.first;
			double c1 = a1 * (A.first) + b1 * (A.second);

			// Line CD represented as a2x + b2y = c2 
			double a2 = D.second - C.second;
			double b2 = C.first - D.first;
			double c2 = a2 * (C.first) + b2 * (C.second);

			// Calc determenant and return true if not 0.
			return (a1 * b2 - a2 * b1) != 0;
		}

		template<typename T, glm::qualifier Q>
		inline glm::vec<2, T, Q> getIntersectPoint(const Line<T, Q>& l1, const Line<T, Q>& l2) {
			// Line AB represented as a1x + b1y = c1 
			double a1 = B.second - A.second;
			double b1 = A.first - B.first;
			double c1 = a1 * (A.first) + b1 * (A.second);

			// Line CD represented as a2x + b2y = c2 
			double a2 = D.second - C.second;
			double b2 = C.first - D.first;
			double c2 = a2 * (C.first) + b2 * (C.second);

			double determinant = a1 * b2 - a2 * b1;

			if (determinant == 0)
			{
				// The lines are parallel. This is simplified 
				// by returning a pair of FLT_MAX 
				return glm::vec<2, T, Q>(FLT_MAX, FLT_MAX);
			}
			else
			{
				double x = (b2 * c1 - b1 * c2) / determinant;
				double y = (a1 * c2 - a2 * c1) / determinant;
				return glm::vec<2, T, Q>(x, y);
			}
		}
#pragma endregion

#pragma region TRIANGLE
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Circle<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Line<T, Q>& l);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t1, const Triangle<T, Q>& t2);
#pragma endregion
	}
}