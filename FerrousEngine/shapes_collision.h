#pragma once
#include "shapes_common.h"

namespace fe {
	namespace shapes {
#pragma region RECT
		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const T x, const T y) {
			return (x >= r.left) && (x <= r.right) && (y >= r.top) && (y <= r.bottom);
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q> & r, const glm::vec<2, T, Q>& v) {
			return (v.x >= r.left) && (v.x <= r.right) && (v.y >= r.top) && (v.y <= r.bottom);
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const Circle<T, Q>& c) {
			T distX = glm::abs(c.origin.x - r.left - r.halfWidth());
			T distY = glm::abs(c.origin.y - r.top - r.halfHeight());

			if (distX > (r.halfWidth() + c.radius) || 
				distY > (r.halfHeight() + c.radius))
				return false;

			if (distX <= r.halfWidth() || 
				distY <= r.halfHeight())
				return true;

			T dx = distX - r.halfWidth();
			T dy = distY - r.halfHeight();
			return (dx * dx + dy * dy <= (c.radius * c.radius));
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const Line<T, Q>& l) {
			return contains(r, l.start) && contains(r, l.end);
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const Triangle<T, Q>& t) {
			return contains(r, t.a) && contains(r, t.b) && contains(r, t.c);
		}

		/* Returns true if the first rectangle (r1) contains the second rectangle (r2). */
		template<typename T, glm::qualifier Q>
		inline bool contains(const Rect<T, Q>& r1, const Rect<T, Q>& r2) {
			return (r2.left >= r1.left) && (r2.right <= r1.right) && (r2.top >= r1.top) && (r2.bottom <= r1.bottom);
		}

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
			if (contains(r, l))
				return true;

			return intersects(r.sideTop(), l) || // Top side
				intersects(r.sideRight(), l) || // Right side
				intersects(r.sideBottom(), l) || // Bottom side
				intersects(r.sideLeft(), l); // Left side
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Triangle<T, Q>& t) {
			return contains(r, t.a) || contains(r, t.b) || contains(r, t.c) ||
				intersects(r, t.sideA()) || intersects(r, t.sideB()) || intersects(r, t.sideC());
		}

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
		inline bool contains(const Circle<T, Q>& c, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Line<T, Q>& l) {
			return (glm::distance(c.origin, l.start) < c.radius) &&
				(glm::distance(c.origin, l.end) < c.radius);
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c1, const Circle<T, Q>& c2);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const T x, const T y) {
			return glm::distance(c.origin, glm::vec<2, T, Q>(x, y)) <= c.radius;
		}

		template<typename T, glm::qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const glm::vec<2, T, Q> v) {
			return glm::distance(c.origin, v) <= c.radius;
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Rect<T, Q>& r) {
			return intersects(r, c);
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Line<T, Q>& l);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Circle<T, Q>& c1, const Circle<T, Q>& c2) {
			return glm::distance(c1.origin, c2.origin) <= (c1.radius + c2.radius);
		}
#pragma endregion

#pragma region LINE
		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Rect<T, Q>& r) {
			return intersects(r, c);
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& l, const Circle<T, Q>& c);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Line<T, Q>& l, const Triangle<T, Q>& t) {
			return contains(t, l) || intersects(t.sideA(), l) || intersects(t.sideB(), l) || intersects(t.sideC(), l);
		}

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
		inline bool contains(const Triangle<T, Q>& t, const T x, const T y);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Triangle<T, Q>& t, const glm::vec<2, T, Q> v);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Triangle<T, Q>& t, const Rect<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Triangle<T, Q>& t, const Line<T, Q>& l);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Triangle<T, Q>& t, const Circle<T, Q>& c);

		template<typename T, glm::qualifier Q>
		inline bool contains(const Triangle<T, Q>& t1, const Triangle<T, Q>& t2);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Rect<T, Q>& r) {
			return intersects(r, t);
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Circle<T, Q>& r);

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Line<T, Q>& l) {
			return intersects(l, t);
		}

		template<typename T, glm::qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t1, const Triangle<T, Q>& t2);
#pragma endregion
	}
}