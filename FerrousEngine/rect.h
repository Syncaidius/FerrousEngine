#pragma once
#include "shapes_setup.h"
#include <algorithm>
using namespace glm;

namespace fe {
	namespace shapes {
		template<typename T, qualifier Q>
		struct Rect {
			T left;
			T top;
			T right;
			T bottom;

			Rect() {
				this->left = (T)0;
				this->top = (T)0;
				this->right = (T)0;
				this->bottom = (T)0;
			}

			Rect(T left, T top, T right, T bottom) {
				assert(right >= left);
				assert(bottom >= top);

				this->left = left;
				this->top = top;
				this->right = right;
				this->bottom = bottom;
			}

			vec<2, T, Q> topLeft() {
				return vec<2, T, Q>(left, top);
			}

			vec<2, T, Q> topRight() {
				return vec<2, T, Q>(right, top);
			}

			vec<2, T, Q> bottomLeft() {
				return vec<2, T, Q>(left, bottom);
			}

			vec<2, T, Q> bottomRight() {
				return vec<2, T, Q>(right, bottom);
			}

			vec<2, T, Q> center() {
				return vec<2, T, Q>(
					left + (right - left) / static_cast<T>(2),
					top + (bottom - top) / static_cast<T>(2));
			}

			T width() {
				return right - left;
			}

			T height() {
				return bottom - top;
			}

			T halfWidth() {
				return width() / (T)2;
			}

			T halfHeight() {
				return height() / (T)2;
			}

			T perimeter() {
				return 2 * (width() + height());
			}

			T area() {
				return width()* height();
			}

			Line<T, Q> sideLeft() {
				return Line<T, Q>(left, top, left, bottom);
			}

			Line<T, Q> sideTop() {
				return Line<T, Q>(left, top, right, top);
			}

			Line<T, Q> sideRight() {
				return Line<T, Q>(right, top, right, bottom);
			}

			Line<T, Q> sideBottom() {
				return Line<T, Q>(left, bottom, right, bottom);
			}

			void inflate(T horizontal, T vertical) {
				left -= horizontal;
				top -= vertical;
				right == horizontal;
				bottom += vertical;
			}

			inline void inflate(T amount) { inflate(amount, amount); }

			Rect<T, Q> getInflated(T horizontal, T vertical) {
				return Rect<T, Q>(
					left - horizontal,
					top - vertical,
					right + horizontal,
					bottom + vertical);
			}

			inline Rect<T, Q> getInflated(T amount) {
				return getInflated(amount, amount);
			}

			/* Creates a new rectangle that exactly contains the current and other rectangle. */
			Rect<T, Q> unify(const Rect<T, Q>& other) {
				return Rect(std::min(left, other.left),
					std::min(top, other.top),
					std::max(right, other.right),
					std::max(bottom, other.bottom));
			}

			/* Returns an empty rectangle. */
			static Rect<T, Q> empty() {
				return Rect<T, Q>();
			}
		};

#pragma region CONTAINMENT
		template<typename T, qualifier Q>
		inline bool contains(const Rect<T, Q>& r, const T x, const T y) {
			return (x >= r.left) && (x <= r.right) && (y >= r.top) && (y <= r.bottom);
		}

		template<typename T, qualifier Q>
		inline bool contains(const Rect<T, Q> & r, const vec<2, T, Q> & v) {
			return (v.x >= r.left) && (v.x <= r.right) && (v.y >= r.top) && (v.y <= r.bottom);
		}

		template<typename T, qualifier Q>
		inline bool contains(const Rect<T, Q> & r, const Circle<T, Q> & c) {
			T distX = abs(c.origin.x - r.left - r.halfWidth());
			T distY = abs(c.origin.y - r.top - r.halfHeight());

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

		template<typename T, qualifier Q>
		inline bool contains(const Rect<T, Q> & r, const Line<T, Q> & l) {
			return contains(r, l.start) && contains(r, l.end);
		}

		template<typename T, qualifier Q>
		inline bool contains(const Rect<T, Q> & r, const Triangle<T, Q> & t) {
			return contains(r, t.a) && contains(r, t.b) && contains(r, t.c);
		}

		/* Returns true if the first rectangle (r1) contains the second rectangle (r2). */
		template<typename T, qualifier Q>
		inline bool contains(const Rect<T, Q> & r1, const Rect<T, Q> & r2) {
			return (r2.left >= r1.left) && (r2.right <= r1.right) && (r2.top >= r1.top) && (r2.bottom <= r1.bottom);
		}
#pragma endregion

#pragma region INTERSECTION
		template<typename T, qualifier Q>
		inline bool intersects(const Rect<T, Q>& r, const Circle<T, Q>& c) {
			T distX = abs(c.origin.x - r.left - r.width() / 2);
			T distY = abs(c.origin.y - r.top - r.height() / 2);

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

		template<typename T, qualifier Q>
		inline bool intersects(const Rect<T, Q> & r, const Line<T, Q> & l) {
			if (contains(r, l))
				return true;

			return intersects(r.sideTop(), l) || // Top side
				intersects(r.sideRight(), l) || // Right side
				intersects(r.sideBottom(), l) || // Bottom side
				intersects(r.sideLeft(), l); // Left side
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Rect<T, Q> & r, const Triangle<T, Q> & t) {
			return contains(r, t.a) || contains(r, t.b) || contains(r, t.c) ||
				intersects(r, t.sideA()) || intersects(r, t.sideB()) || intersects(r, t.sideC());
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Rect<T, Q> & r1, const Rect<T, Q> & r2) {
			return !((r1.left < r2.right) && (r1.right > r2.left) && (r1.top > r2.bottom) && (r1.bottom < r2.top));
		}

		/* Returns a new rectangle representing the area of intersection between two rectangles */
		template<typename T, qualifier Q>
		inline Rect<T, Q> getIntersectArea(const Rect<T, Q> & r1, const Rect<T, Q> & r2) {
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

#pragma region OPERATORS
		template<typename T, qualifier Q>
		inline bool operator ==(const Rect<T, Q>& l, const Rect<T, Q>& r) {
			return l.left == r.left && l.top == r.top && l.right == r.right && l.bottom == r.bottom;
		}
#pragma endregion

#pragma region RECTANGLE TYPES
		/*16-bit signed integer rectangle.*/
		typedef Rect<int16_t, highp> Rect16;

		/*32-bit signed integer rectangle.*/
		typedef Rect<int32_t, highp> Rect32;

		/*64-bit integer rectangle.*/
		typedef Rect<int64_t, highp> Rect64;

		/*16-bit unsigned integer rectangle.*/
		typedef Rect<uint16_t, highp> RectU16;

		/*32-bit unsigned integer rectangle.*/
		typedef Rect<uint32_t, highp> URect32;

		/*64-bit unsigned integer rectangle.*/
		typedef Rect<uint64_t, highp> URect64;

		/*32-bit single-precision floating-point rectangle.*/
		typedef Rect<float, defaultp> RectF;

		/*64-bit double-precision, floating-point rectangle.*/
		typedef Rect<double, highp> RectD;
#pragma endregion
	}
}