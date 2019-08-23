#pragma once
#include "shapes_common.h"

namespace fe {
	namespace shapes {
#pragma region RECT
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Circle<T, Q>& c) {
			T distX = glm::abs(c.origin.x - r.left - r.width() / 2);
			T distY = glm::abs(c.origin.y - r.top - r.height() / 2);

			if (distX > (r.width() / 2 + c.radius))
				return false;

			if (distY > (r.height() / 2 + c.radius))
				return false;

			if (distX <= (r.width() / 2))
				return true;

			if (distY <= (r.height() / 2))
				return true;

			T dx = distX - r.width() / (T)2;
			T dy = distY - r.height() / (T)2;
			return (dx * dx + dy * dy <= (c.radius * c.radius));
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Line<T, Q>& l) {
			return intersects(r.sideTop(), l) || // Top side
				intersects(r.sideRight(), l) || // Right side
				intersects(r.sideBottom(), l) || // Bottom side
				intersects(r.sideLeft(), l); // Left side
		}

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
		inline bool intersects(const Circle<T, Q>& c, const Rect<T, Q>& r) {
			return intersects(r, c);
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Line<T, Q>& l);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c1, const Circle<T, Q>& c2);
#pragma endregion

#pragma region LINE
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Rect<T, Q>& r) {
			return intersects(r, c);
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Circle<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& l1, const Line<T, Q>& l2) {
			// Line AB represented as a1x + b1y = c1 
			double a1 = l1.end.y - l1.start.y;
			double b1 = l1.start.x - l1.end.x;
			double c1 = a1 * (l1.start.x) + b1 * (l1.start.y);

			// Line CD represented as a2x + b2y = c2 
			double a2 = l2.end.y - l2.start.y;
			double b2 = l2.start.x - l2.end.x;
			double c2 = a2 * (l2.start.x) + b2 * (l2.start.y);

			// Calc determenant and return true if not 0.
			return (a1 * b2 - a2 * b1) != 0;
		}

		template<typename T, glm::qualifier Q>
		inline glm::vec<2, T, Q> getIntersectPoint(const Line<T, Q>& l1, const Line<T, Q>& l2) {
			// Line l1 represented as a1x + b1y = c1 
			double a1 = l1.end.y - l1.start.y;
			double b1 = l1.start.x - l1.end.x;
			double c1 = a1 * (l1.start.x) + b1 * (l1.start.y);

			// Line l2 represented as a2x + b2y = c2 
			double a2 = l2.end.y - l2.start.y;
			double b2 = l2.start.x - l2.end.x;
			double c2 = a2 * (l2.start.x) + b2 * (l2.start.y);

			double determinant = a1 * b2 - a2 * b1;

			if (determinant == 0)
			{
				// The lines are parallel. This is simplified 
				// by returning a pair of FLT_MAX 
				return glm::vec<2, T, Q>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
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